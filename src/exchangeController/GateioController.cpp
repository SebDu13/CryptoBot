#include "exchangeController/GateioController.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include <set>
#include <optional>

namespace{
const std::string idPattern = "\"id\":\"";
const std::string basePattern = "\"base\":\"";
const std::string quotePattern = "\"quote\":\"";

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
    CHRONO_THIS_SCOPE;
  
    offset = result.find("{", offset);
    if(offset != std::string::npos)
    {
        if(std::size_t next_braket_position = result.find("}", offset); next_braket_position != std::string::npos)
        {
            // here pair has to look like: {"id":"IHT_ETH","base":"IHT"," ... }
            const std::string pair = result.substr(offset, next_braket_position-offset);
            LOG_DEBUG << "pair " << pair;
            std::string id = getFieldFromPattern(pair, idPattern);
            std::string base = getFieldFromPattern(pair, basePattern);
            std::string quote = getFieldFromPattern(pair, quotePattern);
            LOG_DEBUG << "id " << id;
            LOG_DEBUG << "base " << base;
            LOG_DEBUG << "quote " << quote;
            offset +=idPattern.length();
            return {id, base, quote};
        }
    }
    return std::nullopt;
    //  LOG_ERROR << "Could not extract a pair from result at position " << offset << " with result = " << result;
    //throw ExchangeController::ExchangeControllerException("extractPairFromJson failed in GateioController. Check log from details");
}

// We want to extract the pair ID
// pattern : {"id":"STRONG_USDT","base":"STRONG","quote":"USDT","fee":"0.2","min_quote_amount":"1","amount_precision":3,"precision":2,"trade_status":"tradable","sell_start":0,"buy_start":0},{"id"
// let's search "id":" and then the next " in order to get SRONG_USDT
std::unordered_set<std::string> extractPairsfromJson(const GateIoCPP::CurrencyPairsResult& result)
{
    CHRONO_THIS_SCOPE;

    std::unordered_set<std::string> setOfPairs;
    std::size_t offset=0;
    while(offset < std::string::npos)
        setOfPairs.emplace(extractPairFromJson(result, offset).id);

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
}

namespace ExchangeController{

GateioController::GateioController(std::string &api_key, std::string &secret_key):gateIoAPI(api_key, secret_key)
{
    GateIoCPP::CurrencyPairsResult result;
    //gateIoAPI.get_currency_pairs(result);
    result = "{\"id\":\"STRONG_USDT\",\"base\":\"STRONG\",\"quote\":\"USDT\",\"fee\":\"0.2\",\"min_quote_amount\":\"1\",\"amount_precision\":3,\"precision\":2,\"trade_status\":\"tradable\",\"sell_start\":0,\"buy_start\":0},{\"id\":\"POUETTE\",\"base\":\"STRONG\",\"quote\":\"USDT\",\"fee\":\"0.2\",\"min_quote_amount\":\"1\",\"amount_precision\":3,\"precision\":2,\"trade_status\":\"tradable\",\"sell_start\":0,\"buy_start\":0}";
    if(!sanityCheck(result))
        throw ExchangeControllerException("Sanity check failed in GateioController()");
    allCurrencyPairsCache = extractPairsfromJson(result);
}

GateioController::~GateioController()
{
}

std::optional<CurrencyPair> GateioController::findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result) const
{
    CHRONO_THIS_SCOPE;

    std::size_t offset=0;
    while(offset < std::string::npos)
    {
        if(auto currencyPair = extractPairFromJson(result, offset); !allCurrencyPairsCache.contains(currencyPair.id))
            return currencyPair;
    }
    return std::nullopt;
}

CurrencyPair GateioController::getNewCurrencyPairSync() 
{
    while(true)
    {
        CHRONO_THIS_SCOPE;
        GateIoCPP::CurrencyPairsResult result;
        gateIoAPI.get_currency_pairs(result);

        if(!sanityCheck(result))
            throw ExchangeControllerException("Sanity check failed in GateioController::getNewCurrencyPairSync()");

        if(result.size() < allCurrencyPairsCache.size())
        {
            LOG_WARNING << "allCurrencyPairsCache looks like not up to date, result.size() < allCurrencyPairsCache.size() "
            << result.size() 
            << "<"
            << allCurrencyPairsCache.size()
            << " reset allCurrencyPairsCache with result";
            allCurrencyPairsCache = extractPairsfromJson(result);
        }
        /*else if(result.size() > allCurrencyPairsCache.size())
        {
            return findNewPairFrom(result);
        }*/
    }
}

TickerResult GateioController::getSpotTicker(std::string& currency_pair) const
{
    return TickerResult();
}

OrderResult GateioController::sendOrder(std::string& currency_pair, const Side side, double quantity, double price) const
{
    return OrderResult();
}

} /* end ExchangeController namespace */ 