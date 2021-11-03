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
        throw("define " + std::string(magic_enum::enum_name(side)) + "here: GateIoCPP::Side convertFrom(ExchangeController::Side side)");
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
            const std::string pair = result.substr(offset, next_braket_position-offset);
            //LOG_DEBUG << "pair " << pair;
            std::string id = getFieldFromPattern(pair, idPattern);
            std::string base = getFieldFromPattern(pair, basePattern);
            std::string quote = getFieldFromPattern(pair, quotePattern);
            //LOG_DEBUG << "id: " << id << " ,base: " << base << " ,quote: " << quote;
            offset +=idPattern.length();
            return std::optional<ExchangeController::CurrencyPair>({id, base, quote});
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
}

namespace ExchangeController{

GateioController::GateioController(std::string &api_key, std::string &secret_key):gateIoAPI(api_key, secret_key)
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

std::optional<CurrencyPair> GateioController::findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result) const
{
    CHRONO_THIS_SCOPE;

    std::size_t offset=0;
    while(offset < std::string::npos)
    {
        if(auto currencyPair = extractPairFromJson(result, offset); !allCurrencyPairsCache.contains(currencyPair->id))
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

        if(result.size() < rawCurrencyPairsResultSize)
        {
            LOG_WARNING << "allCurrencyPairsCache looks like not up to date, result.size() < rawCurrencyPairsResultSize "
            << result.size() 
            << "<"
            << rawCurrencyPairsResultSize
            << " reset allCurrencyPairsCache with result";
            allCurrencyPairsCache = extractPairsfromJson<decltype(allCurrencyPairsCache)>(result);
            rawCurrencyPairsResultSize = result.size();
        }
        else if(result.size() > rawCurrencyPairsResultSize)
        {
            if(const auto& newPair = findNewPairFrom(result))
                return *newPair;
            else
            {
                LOG_ERROR << "Could not find new pair while result and allCurrencyParisCache sizes are different: "<< result.size() << " vs " << rawCurrencyPairsResultSize 
                    << std::endl << "result: " << result;
                throw ExchangeController::ExchangeControllerException("findNewPairFrom failed in GateioController. Check logs from details");
            }
        }
    }
}

TickerResult GateioController::getSpotTicker(const std::string& currencyPair) const
{
    CHRONO_THIS_SCOPE;
    GateIoCPP::SpotTickersResult result;
    gateIoAPI.get_spot_tickers(currencyPair, result);

    return {result[0]["last"].asString()
        , result[0]["high_24h"].asString()
        , result[0]["low_24h"].asString()
        , result[0]["base_volume"].asString()
        , result[0]["quote_volume"].asString()
        , result[0]["lowest_ask"].asString()
        , result[0]["highest_bid"].asString()};
}

OrderResult GateioController::sendOrder(const std::string& currencyPair, const Side side, size_t quantity, const std::string& price) const
{
    Json::Value result;
    gateIoAPI.send_limit_order(currencyPair, convertFrom(side), GateIoCPP::TimeInForce::ioc, quantity, price, result);
    LOG_DEBUG << result;
    return OrderResult();
}

} /* end ExchangeController namespace */ 