#pragma once

#include "cct_json.hpp"
#include "curlhandle.hpp"
#include "apikey.hpp"

namespace cct {

class CoincenterInfo;
class ExchangeInfo;
class FiatConverter;

namespace api {
class KucoinPublic;

class KucoinPrivate 
{
 public:
  KucoinPrivate(const CoincenterInfo& config, KucoinPublic& kucoinPublic, const APIKey& apiKey);

  json queryAccountBalance(CurrencyCode equiCurrency = CurrencyCode::kNeutral) ;

 protected:
  json placeOrder(MonetaryAmount from, MonetaryAmount volume, MonetaryAmount price,
                            const TradeInfo& tradeInfo) ;

  OrderInfo cancelOrder(const OrderId& orderId, const TradeInfo& tradeInfo) ;

  OrderInfo queryOrderInfo(const OrderId& orderId, const TradeInfo& tradeInfo) ;

  InitiatedWithdrawInfo launchWithdraw(MonetaryAmount grossAmount, Wallet&& wallet) ;

  SentWithdrawInfo isWithdrawSuccessfullySent(const InitiatedWithdrawInfo& initiatedWithdrawInfo) ;

  bool isWithdrawReceived(const InitiatedWithdrawInfo& initiatedWithdrawInfo,
                          const SentWithdrawInfo& sentWithdrawInfo) ;

 private:

    CurlHandle& _curlHandle;
    const APIKey& _apiKey;
    const KucoinPublic& _kucoinPublic;
  };

  CurlHandle _curlHandle;
};
}  // namespace api
}  // namespace cct