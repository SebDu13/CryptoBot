#include "kucoinprivateapi.hpp"

#include "apikey.hpp"
#include "cct_codec.hpp"
#include "cct_nonce.hpp"
#include "cct_toupperlower.hpp"
#include "ssl_sha.hpp"
#include "BotType.hpp"

namespace cct {
namespace api {

namespace {

json PrivateQuery(CurlHandle& curlHandle, const APIKey& apiKey, CurlOptions::RequestType requestType,
                  std::string_view method, CurlPostData&& postdata = CurlPostData()) {
  const bool endpointWithParameters =
      requestType == CurlOptions::RequestType::kGet || requestType == CurlOptions::RequestType::kDelete;

  CurlOptions opts(requestType, std::move(postdata));

  Nonce nonce = Nonce_TimeSinceEpoch();
  string strToSign = nonce;
  strToSign.append(opts.requestTypeStr());
  strToSign.append(method);

  if (!opts.postdata.empty()) {
    if (endpointWithParameters) {
      strToSign.push_back('?');
      strToSign.append(opts.postdata.str());
    } else {
      strToSign.append(opts.postdata.toJson().dump());
      opts.httpHeaders.emplace_back("Content-Type: application/json");
      opts.postdataInJsonFormat = true;
    }
  }

  string signature = B64Encode(ssl::ShaBin(ssl::ShaType::kSha256, strToSign, apiKey.privateKey()));
  string passphrase = B64Encode(ssl::ShaBin(ssl::ShaType::kSha256, apiKey.passphrase(), apiKey.privateKey()));

  opts.userAgent = KucoinPublic::kUserAgent;

  opts.httpHeaders.emplace_back("KC-API-KEY: ").append(apiKey.key());
  opts.httpHeaders.emplace_back("KC-API-SIGN: ").append(signature);
  opts.httpHeaders.emplace_back("KC-API-TIMESTAMP: ").append(nonce);
  opts.httpHeaders.emplace_back("KC-API-PASSPHRASE: ").append(passphrase);
  opts.httpHeaders.emplace_back("KC-API-KEY-VERSION: 2");

  string url = KucoinPublic::kUrlBase;
  url.append(method);

  json dataJson = json::parse(curlHandle.query(url, opts));
  auto errCodeIt = dataJson.find("code");
  if (errCodeIt != dataJson.end() && errCodeIt->get<std::string_view>() != "200000") {
    std::string errStr("Kucoin error ");
    errStr.append(errCodeIt->get<std::string_view>());
    auto msgIt = dataJson.find("msg");
    if (msgIt != dataJson.end()) {
      errStr.append(" - ");
      errStr.append(msgIt->get<std::string_view>());
    }
    if (requestType == CurlOptions::RequestType::kDelete) {
      log::warn("{} bypassed, object probably disappeared correctly", errStr);
      dataJson.clear();
      return dataJson;
    }
    throw exception(std::move(errStr));
  }
  return dataJson["data"];
}

void InnerTransfer(CurlHandle& curlHandle, const APIKey& apiKey, MonetaryAmount amount, std::string_view fromStr,
                   std::string_view toStr) {
  log::info("Perform inner transfer of {} to {} account", amount.str(), toStr);
  PrivateQuery(curlHandle, apiKey, CurlOptions::RequestType::kPost, "/api/v2/accounts/inner-transfer",
               {{"clientOid", Nonce_TimeSinceEpoch()},  // Not really needed, but it's mandatory apparently
                {"currency", amount.currencyCode().str()},
                {"amount", amount.amountStr()},
                {"from", fromStr},
                {"to", toStr}});
}

bool EnsureEnoughAmountIn(CurlHandle& curlHandle, const APIKey& apiKey, MonetaryAmount expectedAmount,
                          std::string_view accountName) {
  // Check if enough balance in the 'accountName' account of Kucoin
  CurrencyCode cur = expectedAmount.currencyCode();
  json balanceCur =
      PrivateQuery(curlHandle, apiKey, CurlOptions::RequestType::kGet, "/api/v1/accounts", {{"currency", cur.str()}});
  MonetaryAmount totalAvailableAmount(0, cur);
  MonetaryAmount amountInTargetAccount = totalAvailableAmount;
  for (const json& balanceDetail : balanceCur) {
    std::string_view typeStr = balanceDetail["type"].get<std::string_view>();
    MonetaryAmount av(balanceDetail["available"].get<std::string_view>(), cur);
    totalAvailableAmount += av;
    if (typeStr == accountName) {
      amountInTargetAccount = av;
    }
  }
  if (totalAvailableAmount < expectedAmount) {
    log::error("Insufficient funds to place in '{}' ({} < {})", accountName, totalAvailableAmount.str(),
               expectedAmount.str());
    return false;
  }
  if (amountInTargetAccount < expectedAmount) {
    for (const json& balanceDetail : balanceCur) {
      std::string_view typeStr = balanceDetail["type"].get<std::string_view>();
      MonetaryAmount av(balanceDetail["available"].get<std::string_view>(), cur);
      if (typeStr != accountName && !av.isZero()) {
        MonetaryAmount remainingAmountToInnerTransfer = expectedAmount - amountInTargetAccount;
        if (av < remainingAmountToInnerTransfer) {
          InnerTransfer(curlHandle, apiKey, av, typeStr, accountName);
          amountInTargetAccount += av;
        } else {
          InnerTransfer(curlHandle, apiKey, remainingAmountToInnerTransfer, typeStr, accountName);
          break;
        }
      }
    }
  }
  return true;
}

}  // namespace

KucoinPrivate::KucoinPrivate(const CoincenterInfo& config, KucoinPublic& kucoinPublic, const APIKey& apiKey)
    : ExchangePrivate(kucoinPublic, config, apiKey),
      _curlHandle(config.exchangeInfo(kucoinPublic.name()).minPrivateQueryDelay(), config.getRunMode()),
 {}

json KucoinPrivate::queryAccountBalance(CurrencyCode equiCurrency) 
{
  return PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kGet, "/api/v1/accounts");
}

Wallet KucoinPrivate::DepositWalletFunc::operator()(CurrencyCode currencyCode) {
  json result = PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kGet, "/api/v2/deposit-addresses",
                             {{"currency", currencyCode.str()}});
  if (result.empty()) {
    log::info("No deposit address for {} in {}, creating one", currencyCode.str(), _kucoinPublic.name());
    result = PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kPost, "/api/v1/deposit-addresses",
                          {{"currency", currencyCode.str()}});
  } else {
    result = result.front();
  }
  PrivateExchangeName privateExchangeName(_kucoinPublic.name(), _apiKey.name());
  std::string_view address = result["address"].get<std::string_view>();
  std::string_view tag = result["memo"].get<std::string_view>();
  std::string_view dataDir = _kucoinPublic.coincenterInfo().dataDir();
  if (!Wallet::IsAddressPresentInDepositFile(dataDir, privateExchangeName, currencyCode, address, tag)) {
    log::warn("{} & tag {} are not validated in the deposit addresses file", address, tag);
    address = std::string_view();
    tag = std::string_view();
  }

  Wallet w(privateExchangeName, currencyCode, address, tag, _kucoinPublic.coincenterInfo());
  log::info("Retrieved {}", w.str());
  return w;
}

json KucoinPrivate::placeOrder(MonetaryAmount from, MonetaryAmount volume, MonetaryAmount price,
                                         const TradeInfo& tradeInfo) 
{

  std::string_view buyOrSell = fromCurrencyCode == m.base() ? "sell" : "buy";
  std::string_view strategyType = isTakerStrategy ? "market" : "limit";
  CurlPostData placePostData{{"clientOid", Nonce_TimeSinceEpoch()},
                             {"side", buyOrSell},
                             {"symbol", m.assetsPairStr('-')},
                             {"type", strategyType},
                             {"tradeType", "TRADE"},
                             {"size", volume.amountStr()}};
  if (!isTakerStrategy) {
    placePostData.append("price", price.amountStr());
  }

  return PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kPost, "/api/v1/orders", std::move(placePostData));

}

OrderInfo KucoinPrivate::cancelOrder(const OrderId& orderId, const TradeInfo& tradeInfo) {
  string endpoint = "/api/v1/orders/";
  endpoint.append(orderId);
  PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kDelete, endpoint);
  return queryOrderInfo(orderId, tradeInfo);
}

OrderInfo KucoinPrivate::queryOrderInfo(const OrderId& orderId, const TradeInfo& tradeInfo) {
  const CurrencyCode fromCurrencyCode(tradeInfo.fromCurrencyCode);
  const Market m = tradeInfo.m;
  string endpoint = "/api/v1/orders/";
  endpoint.append(orderId);

  json data = PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kGet, endpoint);

  MonetaryAmount size(data["size"].get<std::string_view>(), m.base());
  MonetaryAmount matchedSize(data["dealSize"].get<std::string_view>(), m.base());

  // Fee is already deduced from the matched amount
  MonetaryAmount fromAmount, toAmount;
  MonetaryAmount dealFunds(data["dealFunds"].get<std::string_view>(), m.quote());
  if (fromCurrencyCode == m.base()) {
    // sell
    fromAmount = matchedSize;
    toAmount = dealFunds;
  } else {
    // buy
    fromAmount = dealFunds;
    toAmount = matchedSize;
  }
  return OrderInfo(TradedAmounts(fromAmount, toAmount), !data["isActive"].get<bool>());
}

InitiatedWithdrawInfo KucoinPrivate::launchWithdraw(MonetaryAmount grossAmount, Wallet&& wallet) {
  if (!EnsureEnoughAmountIn(_curlHandle, _apiKey, grossAmount, "main")) {
    throw exception("Insufficient funds for withdraw");
  }
  const CurrencyCode currencyCode = grossAmount.currencyCode();

  MonetaryAmount fee(_exchangePublic.queryWithdrawalFee(grossAmount.currencyCode()));
  MonetaryAmount netEmittedAmount = grossAmount - fee;

  CurlPostData opts{
      {"currency", currencyCode.str()}, {"address", wallet.address()}, {"amount", netEmittedAmount.amountStr()}};
  if (wallet.hasDestinationTag()) {
    opts.append("memo", wallet.destinationTag());
  }

  json result =
      PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kPost, "/api/v1/withdrawals", std::move(opts));
  return InitiatedWithdrawInfo(std::move(wallet), result["withdrawalId"].get<std::string_view>(), grossAmount);
}

SentWithdrawInfo KucoinPrivate::isWithdrawSuccessfullySent(const InitiatedWithdrawInfo& initiatedWithdrawInfo) {
  const CurrencyCode currencyCode = initiatedWithdrawInfo.grossEmittedAmount().currencyCode();
  std::string_view withdrawId = initiatedWithdrawInfo.withdrawId();
  json withdrawJson = PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kGet, "/api/v1/withdrawals",
                                   {{"currency", currencyCode.str()}});
  MonetaryAmount netEmittedAmount;
  bool isWithdrawSent = false;
  for (const json& withdrawDetail : withdrawJson["items"]) {
    if (withdrawDetail["id"].get<std::string_view>() == withdrawId) {
      std::string_view withdrawStatus = withdrawDetail["status"].get<std::string_view>();
      if (withdrawStatus == "PROCESSING") {
        log::debug("Processing");
      } else if (withdrawStatus == "WALLET_PROCESSING") {
        log::debug("Wallet processing");
      } else if (withdrawStatus == "SUCCESS") {
        log::debug("Success");
        isWithdrawSent = true;
      } else if (withdrawStatus == "FAILURE") {
        log::warn("Failure");
      } else {
        log::error("unknown status value '{}'", withdrawStatus);
      }
      netEmittedAmount = MonetaryAmount(withdrawDetail["amount"].get<std::string_view>(), currencyCode);
      MonetaryAmount fee(withdrawDetail["fee"].get<std::string_view>(), currencyCode);
      if (netEmittedAmount + fee != initiatedWithdrawInfo.grossEmittedAmount()) {
        log::error("{} + {} != {}, maybe a change in API", netEmittedAmount.amountStr(), fee.amountStr(),
                   initiatedWithdrawInfo.grossEmittedAmount().amountStr());
      }
      break;
    }
  }
  return SentWithdrawInfo(netEmittedAmount, isWithdrawSent);
}

bool KucoinPrivate::isWithdrawReceived(const InitiatedWithdrawInfo& initiatedWithdrawInfo,
                                       const SentWithdrawInfo& sentWithdrawInfo) {
  const CurrencyCode currencyCode = initiatedWithdrawInfo.grossEmittedAmount().currencyCode();

  json depositJson = PrivateQuery(_curlHandle, _apiKey, CurlOptions::RequestType::kGet, "/api/v1/deposits",
                                  {{"currency", currencyCode.str()}});
  MonetaryAmount netEmittedAmount = sentWithdrawInfo.netEmittedAmount();
  for (const json& depositDetail : depositJson["items"]) {
    MonetaryAmount amount(depositDetail["amount"].get<std::string_view>(), currencyCode);
    if (amount == netEmittedAmount) {
      std::string_view depositStatus = depositDetail["status"].get<std::string_view>();
      if (depositStatus == "PROCESSING") {
        log::debug("Processing");
      } else if (depositStatus == "SUCCESS") {
        log::debug("Success");
        return true;
      } else if (depositStatus == "FAILURE") {
        log::debug("Failure");
      } else {
        log::error("unknown status value '{}'", depositStatus);
      }
      break;
    }
  }
  return false;
}
}  // namespace api
}  // namespace cct