#include "exchangeController/GateioController.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include <set>

namespace{
    std::unordered_set<std::string> extractPairsfromJson(const Json::Value& jsonPairs)
    {
        CHRONO_THIS_SCOPE;

        std::unordered_set<std::string> setPairs;
        for(const auto& pair: jsonPairs)
            setPairs.emplace(pair["id"].toStyledString());
        
        return setPairs;
    }

    bool sanityCheck(const GateIoCPP::CurrencyPairsResult& result)
    {  
        // Let's just check some fields
        return result.size() > 1
            && !result[1].get("id", Json::Value()).empty()
            && !result[1].get("base", Json::Value()).empty()
            && !result[1].get("quote", Json::Value()).empty()
            && !result[1].get("free", Json::Value()).empty();
    }
}

namespace ExchangeController{

GateioController::GateioController(std::string &api_key, std::string &secret_key):gateIoAPI(api_key, secret_key)
{
    Json::Value result;
    gateIoAPI.get_currency_pairs(result);
    if(!sanityCheck(result))
        throw ExchangeControllerException("Sanity check failed in GateioController()");

    allCurrencyPairsCache = extractPairsfromJson(result);
}

GateioController::~GateioController()
{
}

CurrencyPair GateioController::findNewPair(const GateIoCPP::CurrencyPairsResult& result) const
{
    CHRONO_THIS_SCOPE;
    for(const auto& pair: result)
    {
        if(!allCurrencyPairsCache.contains(pair["id"].toStyledString()))
        {
            return {pair["id"].toStyledString()
                , pair["base"].toStyledString()
                , pair["quote"].toStyledString()};
        }
    }
    throw ExchangeControllerException("New pair not found in GateioController::findNewPair");
}

CurrencyPair GateioController::getNewCurrencyPairSync() 
{
    CHRONO_THIS_SCOPE;

    Json::Value result;
    while(true)
    {
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
        else if(result.size() > allCurrencyPairsCache.size())
        {
            return findNewPair(result);
        }
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