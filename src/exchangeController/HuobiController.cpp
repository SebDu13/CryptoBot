#include "exchangeController/HuobiController.hpp"
#include "chrono.hpp"
#include "logger.hpp"

namespace ExchangeController{

HuobiController::HuobiController(const Bot::ApiKeys& apiKeys):_huobiApi(apiKeys.pub, apiKeys.secret, apiKeys.passphrase)
{
}

HuobiController::~HuobiController()
{
}

TickerResult HuobiController::getSpotTicker(const std::string& currencyPair) const
{
    //CHRONO_THIS_SCOPE;
    static double high_24h = 0; // artificial
    Json::Value result;
	_huobiApi.getTicker(currencyPair, result);
	LOG_INFO << result;
    TickerResult tickerResult;
    try
    {
        auto& tick = result["tick"];
        double last = boost::lexical_cast<double>(tick["bid"][0].asString());
        double high24 = boost::lexical_cast<double>(tick["high"].asString());

        if(high24 == 0)
        {
            LOG_WARNING << "high24 equal 0";
            if(last > high_24h)
                high_24h = last;
            
            high24 = high_24h;
        }

        tickerResult = { .last = last
            , .high24h = high24
            , .low24h = boost::lexical_cast<double>(tick["low"].asString())
            , .baseVolume = boost::lexical_cast<double>(tick["amount"].asString())
            , .quoteVolume = boost::lexical_cast<double>(tick["vol"].asString())
            , .lowestAsk = boost::lexical_cast<double>(tick["ask"][0].asString())
            , .highestBid = boost::lexical_cast<double>(tick["bid"][0].asString())};
    }
    catch(std::exception& e)
    {
        LOG_ERROR << "std::exception caught: " << e.what();
        sleep(1); // in case order api limit is reached
        return {};
    }
    catch(...)
    {
        LOG_ERROR << "Unknown exception caught";
        sleep(1);
        return {};
    }
    
    return tickerResult;
}

std::string HuobiController::getOrderBook(const std::string& currencyPair) const
{
    return std::string();
}

OrderResult HuobiController::sendOrder(const std::string& currencyPair, const Side side, const Quantity& quantity, const Price& price) const
{

}

Quantity HuobiController::computeMaxAmount(const Price& price) const
{
    return Quantity{};
}

Quantity HuobiController::getMinOrderSize() const
{
    return Quantity{"1"};
}

Quantity HuobiController::getAmountLeft(const OrderResult& buyOrderResult) const
{
    return buyOrderResult.amount;
}

Quantity HuobiController::prepareAccount(const Price& price,const std::optional<Quantity>& maxAmount, const std::optional<Quantity>& quantity) const
{
    return computeMaxAmount(price);
}

}