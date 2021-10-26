#include "exchangeController/GateioController.hpp"
#include "chrono.hpp"
#include "logger.hpp"

namespace{
    std::unordered_set<std::string> extractPairsfromJson(const Json::Value& jsonPairs)
    {
        CHRONO_THIS_FUNCTION;

        std::unordered_set<std::string>& setPairs;
        for(const auto& pair: jsonPairs)
            setPairs.emplace(pair["id"]);
        
        return setPairs;
    }
}

namespace ExchangeController{

GateioController::GateioController(std::string &api_key, std::string &secret_key):gateIoAPI(api_key, secret_key)
{
    Json::Value result;
    gateIoAPI.get_currency_pairs(result);
    allCurrencyPairsCache = extractPairsfromJson(result);
}

GateioController::~GateioController()
{
}

CurrencyPair GateioController::getNewCurrencyPairSync() const 
{
    CHRONO_THIS_FUNCTION;

    Json::Value result;
    while(result.size() <= allCurrencyPairsCache.size())
    {
        gateIoAPI.get_currency_pairs(result); // gestion d'erreur ici

        if(result.size())
        if(result.size() < allCurrencyPairsCache.size())
        {
            LOG_WARNING << "allCurrencyPairsCache looks like not up to date, result.size() < allCurrencyPairsCache.size() "
            << result.size() 
            << "<"
            << allCurrencyPairsCache.size()
            << " reset allCurrencyPairsCache with result";
            allCurrencyPairsCache = extractPairsfromJson(result);
        }
    }
}

TickerResult GateioController::getSpotTickerSync(std::string& currency_pair) const
{

}

OrderResult GateioController::sendOrder(std::string& currency_pair, const Side side, double quantity, double price) const
{

}

} /* end ExchangeController namespace */ 