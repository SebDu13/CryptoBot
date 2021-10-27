#include "exchangeController/GateioController.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include <set>

namespace{

    // We want to extract the pair ID
    // pattern : {"id":"STRONG_USDT","base":"STRONG","quote":"USDT","fee":"0.2","min_quote_amount":"1","amount_precision":3,"precision":2,"trade_status":"tradable","sell_start":0,"buy_start":0},{"id"
    // let's search "id":" and then the next " in order to get SRONG_USDT
    std::unordered_set<std::string> extractPairsfromJson(const GateIoCPP::CurrencyPairsResult& result)
    {
        CHRONO_THIS_SCOPE;

        std::unordered_set<std::string> setPairs;
        std::string idPattern = "\"id\":\"";
        std::size_t id_position=0, next_quote_position = 0;        
        while(id_position < std::string::npos)
        {
            id_position = result.find(idPattern, id_position);
            if(id_position != std::string::npos)
            {
                next_quote_position = result.find("\"", id_position + idPattern.length());
                setPairs.emplace(result.substr(id_position + idPattern.length(), next_quote_position - (idPattern.length()+id_position)));
                id_position +=idPattern.length();
            }
        }

        return setPairs;
    }

    bool sanityCheck(const GateIoCPP::CurrencyPairsResult& result)
    {  
        // Let's just check some fields
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
    if(!sanityCheck(result))
        throw ExchangeControllerException("Sanity check failed in GateioController()");
    //result = "{\"id\":\"STRONG_USDT\",\"base\":\"STRONG\",\"quote\":\"USDT\",\"fee\":\"0.2\",\"min_quote_amount\":\"1\",\"amount_precision\":3,\"precision\":2,\"trade_status\":\"tradable\",\"sell_start\":0,\"buy_start\":0},{\"id\":\"POUETTE\",\"base\":\"STRONG\",\"quote\":\"USDT\",\"fee\":\"0.2\",\"min_quote_amount\":\"1\",\"amount_precision\":3,\"precision\":2,\"trade_status\":\"tradable\",\"sell_start\":0,\"buy_start\":0}";
    allCurrencyPairsCache = extractPairsfromJson(result);
}

GateioController::~GateioController()
{
}

CurrencyPair GateioController::findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result) const
{
    CHRONO_THIS_SCOPE;

    std::unordered_set<std::string> setPairs;
    std::string idPattern = "\"id\":\"";
    std::size_t id_position=0, next_quote_position = 0;        
    while(id_position < std::string::npos)
    {
        id_position = result.find(idPattern, id_position);
        if(id_position != std::string::npos)
        {
            next_quote_position = result.find("\"", id_position + idPattern.length());
            std::string pairName = result.substr(id_position + idPattern.length(), next_quote_position - (idPattern.length()+id_position));
            if(!allCurrencyPairsCache.contains(pairName))
                return CurrencyPair{};
            id_position +=idPattern.length();
        }
    }

    return CurrencyPair{};
}

/*CurrencyPair GateioController::findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result) const
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
}*/

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