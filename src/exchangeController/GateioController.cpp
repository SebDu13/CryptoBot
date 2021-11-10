#include "exchangeController/GateioController.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include <optional>
#include "boost/lexical_cast.hpp"
#include "magic_enum.hpp"

namespace{
const std::string idPattern = "\"id\":\"";
const std::string basePattern = "\"base\":\"";
const std::string quotePattern = "\"quote\":\"";

GateIoCPP::Side convertFrom(ExchangeController::Side side)
{
    switch (side)
    {
    case ExchangeController::Side::buy: return GateIoCPP::Side::buy;
    case ExchangeController::Side::sell: return GateIoCPP::Side::sell;
    default:
        throw ExchangeController::ExchangeControllerException("Define " + std::string(magic_enum::enum_name(side)) + "here: GateIoCPP::Side convertFrom(ExchangeController::Side side)");
    }
}

// in the sequence below, for an "id" pattern we want to return IHT_ETH
//{"id":"IHT_ETH","base":"IHT","quote":"ETH","fee":"0.2","min_quote_amount":"0.001","amount_precision":1,"precision":9,"trade_status":"tradable","sell_start":0,"buy_start":0}
std::string getFieldFromPattern(const std::string& input, const std::string& Pattern)
{
    if(std::size_t patternPos = input.find(Pattern, 0); patternPos != std::string::npos)
        if(std::size_t nextQuotePos = input.find("\"", patternPos + Pattern.length()); nextQuotePos != std::string::npos)
            return input.substr(patternPos + Pattern.length(), nextQuotePos - (Pattern.length()+patternPos));

    return "";
}

std::optional<ExchangeController::CurrencyPair> extractPairFromJson(const GateIoCPP::CurrencyPairsResult& result, std::size_t& offset)
{
    offset = result.find("{", offset);
    if(offset != std::string::npos)
    {
        if(std::size_t next_braket_position = result.find("}", offset); next_braket_position != std::string::npos)
        {
            // here pair has to look like: {"id":"IHT_ETH","base":"IHT"," ... }
            const std::string pair(result.substr(offset, next_braket_position-offset));
            offset +=idPattern.length();
            return std::optional<ExchangeController::CurrencyPair>({getFieldFromPattern(pair, idPattern)
                , getFieldFromPattern(pair, basePattern)
                , getFieldFromPattern(pair, quotePattern)});
        }
    }
    return std::nullopt;
}

// We want to extract the pair ID
// pattern : {"id":"STRONG_USDT","base":"STRONG","quote":"USDT","fee":"0.2","min_quote_amount":"1","amount_precision":3,"precision":2,"trade_status":"tradable","sell_start":0,"buy_start":0},{"id"
// let's search "id":" and then the next " in order to get SRONG_USDT
template<typename Contener>
Contener extractPairsfromJson(const GateIoCPP::CurrencyPairsResult& result)
{
    CHRONO_THIS_SCOPE;

    Contener setOfPairs;
    std::size_t offset=0;
    while(offset < std::string::npos)
    {
        if(const auto& pairFromJsonOpt = extractPairFromJson(result, offset))
            setOfPairs.emplace(pairFromJsonOpt->id);
        else if(offset < std::string::npos)
        {
            LOG_ERROR << "Could not extract a pair from result at position " << offset << " with result = " << result;
            throw ExchangeController::ExchangeControllerException("extractPairsfromJson failed in GateioController. Check logs from details");
        }
    }

    return setOfPairs;
}

bool sanityCheck(const GateIoCPP::CurrencyPairsResult& result)
{  
    // Let's check just some fields
    return result.length() > 1
        && result.find("\"id\"", 0) != std::string::npos
        && result.find("\"base\"", 0) != std::string::npos
        && result.find("\"quote\"", 0) != std::string::npos
        && result.find("\"fee\"", 0) != std::string::npos;
}

ExchangeController::OrderStatus fillOrderStatus(const Json::Value& result)
{
    // error Json contain message field
    if(const auto message = result.get("message", Json::Value()); !message.empty())
    {
        //"label" : "INVALID_CURRENCY",
        //"message" : "PIZA_USDT trade is disabled until 2021-11-05T18:00+08:00[Asia/Shanghai]"
        if(message.toStyledString().find("trade is disabled until", 0) != std::string::npos)
            return ExchangeController::OrderStatus::InvalidCurrency;
        //"message" : "Invalid currency CURRENCY_NAME"
        if(message.toStyledString().find("Invalid currency", 0) != std::string::npos)
            return ExchangeController::OrderStatus::InvalidCurrency;
        //"message" : "Your order size 10 is too small. The minimum is 1 USDT"
        if(message.toStyledString().find("too small", 0) != std::string::npos)
            return ExchangeController::OrderStatus::SizeTooSmall;
        //"message" : "Not enough balance"
        if(message.toStyledString().find("Not enough balance", 0) != std::string::npos)
            return ExchangeController::OrderStatus::NotEnoughBalance;
        //"message" : "Your order size 1 is too large. The maximum is 1000000 USDT"
        if(message.toStyledString().find("is too large. The maximum is", 0) != std::string::npos)
            return ExchangeController::OrderStatus::OrderSizeTooLarge;
    }
    if(const auto message = result.get("status", Json::Value()); !message.empty())
    {
        if(message.toStyledString().find("cancelled", 0) != std::string::npos)
            return ExchangeController::OrderStatus::Cancelled;
        if(message.toStyledString().find("closed", 0) != std::string::npos)
            return ExchangeController::OrderStatus::Closed;
    }
    return ExchangeController::OrderStatus::Unknown;
}
}

namespace ExchangeController{

GateioController::GateioController(const Bot::ApiKeys& apiKeys):gateIoAPI(apiKeys.pub, apiKeys.secret)
{
    GateIoCPP::CurrencyPairsResult result;
    gateIoAPI.get_currency_pairs(result);
    //result = "{\"id\":\"STRONG_USDT\",\"base\":\"STRONG\",\"quote\":\"USDT\",\"fee\":\"0.2\",\"min_quote_amount\":\"1\",\"amount_precision\":3,\"precision\":2,\"trade_status\":\"tradable\",\"sell_start\":0,\"buy_start\":0},{\"id\":\"POUETTE\",\"base\":\"STRONG\",\"quote\":\"USDT\",\"fee\":\"0.2\",\"min_quote_amount\":\"1\",\"amount_precision\":3,\"precision\":2,\"trade_status\":\"tradable\",\"sell_start\":0,\"buy_start\":0}";
    if(!sanityCheck(result))
        throw ExchangeControllerException("Sanity check failed in GateioController()");
    
    allCurrencyPairsCache = extractPairsfromJson<decltype(allCurrencyPairsCache)>(result);
    rawCurrencyPairsResultSize = result.size();
    LOG_INFO << allCurrencyPairsCache.size() << " currency pairs listed on GateIO"; 
}

GateioController::~GateioController()
{
}

// We want to find the first pair quoted in USDT. New pairs are generally quoted in USDT and ETH
std::optional<CurrencyPair> GateioController::findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result, const std::string& quote) const
{
    CHRONO_THIS_SCOPE;

    std::size_t offset=0;
    while(offset < std::string::npos)
    {
        if(auto currencyPair = extractPairFromJson(result, offset); !allCurrencyPairsCache.contains(currencyPair->id))
            if(currencyPair->id.find(quote) != std::string::npos)
                return currencyPair;
            else
                LOG_INFO << "New pair[" << currencyPair->id <<"] found but not quoted on " << quote;
    }
    return std::nullopt;
}

CurrencyPair GateioController::getNewCurrencyPairSync(const std::string& quote)  const
{
    //int i = 0;
    while(true)
    {
        CHRONO_THIS_SCOPE;
        GateIoCPP::CurrencyPairsResult result;
        gateIoAPI.get_currency_pairs(result);

        /*++i;
        if(i == 4)
        {
            result += ",{\"id\":\"POUETTE_USDT\",\"base\":\"STRONG\",\"quote\":\"USDT\",\"fee\":\"0.2\",\"min_quote_amount\":\"1\",\"amount_precision\":3,\"precision\":2,\"trade_status\":\"tradable\",\"sell_start\":0,\"buy_start\":0}";
        }*/

        if(!sanityCheck(result))
            throw ExchangeControllerException("Sanity check failed in GateioController::getNewCurrencyPairSync()");

        /*if(result.size() < rawCurrencyPairsResultSize)
        {
            LOG_WARNING << "allCurrencyPairsCache looks like not up to date, result.size() < rawCurrencyPairsResultSize "
            << result.size() 
            << "<"
            << rawCurrencyPairsResultSize
            << " reset allCurrencyPairsCache with result";
            allCurrencyPairsCache = extractPairsfromJson<decltype(allCurrencyPairsCache)>(result);
            rawCurrencyPairsResultSize = result.size();
        }*/
        if(result.size() > rawCurrencyPairsResultSize)
            if(const auto& newPair = findNewPairFrom(result, quote))
                return *newPair;
    }
}

TickerResult GateioController::getSpotTicker(const std::string& currencyPair) const
{
    //CHRONO_THIS_SCOPE;
    GateIoCPP::SpotTickersResult result;
    gateIoAPI.get_spot_tickers(currencyPair, result);

    return {boost::lexical_cast<double>(result[0]["last"].asString())
        , boost::lexical_cast<double>(result[0]["high_24h"].asString())
        , boost::lexical_cast<double>(result[0]["low_24h"].asString())
        , boost::lexical_cast<double>(result[0]["base_volume"].asString())
        , boost::lexical_cast<double>(result[0]["quote_volume"].asString())
        , boost::lexical_cast<double>(result[0]["lowest_ask"].asString())
        , boost::lexical_cast<double>(result[0]["highest_bid"].asString())};
}

std::string GateioController::getOrderBook(const std::string& currencyPair) const
{
    std::string result;
    gateIoAPI.getOrderBook(currencyPair,result);
    return result;
}

OrderResult GateioController::sendOrder(const std::string& currencyPair, const Side side, double quantity, double price) const
{
    Json::Value result;
    gateIoAPI.send_limit_order(currencyPair, convertFrom(side), GateIoCPP::TimeInForce::ioc, quantity, price, result);
    const auto& status = fillOrderStatus(result);
    
    LOG_DEBUG << result;
        
    if( status == OrderStatus::Closed || status == OrderStatus::Cancelled)
        return {status
        ,boost::lexical_cast<double>(result["fill_price"].asString())
        ,boost::lexical_cast<double>(result["filled_total"].asString())
        ,boost::lexical_cast<double>(result["amount"].asString())
        ,boost::lexical_cast<double>(result["fee"].asString()) };
    else
        return {status, 0, 0, 0, 0};
}

double GateioController::getMinPrice() const
{
    return MIN_ORDER_PRICE;
}

Bot::Quantity GateioController::computeMaxQuantity(double price) const
{
    Json::Value result;
    gateIoAPI.getAccountBalances(result);

    if(result["details"]["spot"]["currency"] == "USDT")
    {
        double quantity = std::stod(result["details"]["spot"]["amount"].asString());
        LOG_INFO << "There is " << quantity << " USDT on spot account";
        return Bot::Quantity{quantity};
    }

    return Bot::Quantity{};
}

} /* end of namespace ExchangeController*/