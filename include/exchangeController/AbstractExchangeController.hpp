#pragma once
#include <string>
#include <stdexcept>

namespace ExchangeController
{
    class ExchangeControllerException : public std::runtime_error
    {
        public:
        ExchangeControllerException(const std::string what): std::runtime_error(what){}
    };

    enum class Side
    {
        buy,
        sell
    };

    enum class OrderStatus
    {
        Cancelled,
        Closed,
        SizeTooSmall,
        NotEnoughBalance,
        Unknown
    };

    struct OrderResult
    {
        OrderStatus status;
        double fillPrice;
        double filledTotal;
        double amount;
        double fee;
    };

    struct TickerResult
    {
        double last;
        double high24h;
        double low24h;
        double baseVolume;
        double quoteVolume;
        double lowestAsk;
        double highestBid;
    };

    struct CurrencyPair
    {
        std::string id;
        std::string base;
        std::string quote; // in general USDT
        //fee
        //min_quote_amount
        //amount_precision
        //precision
        //trade_status
        //sell_start
        //buy_start

        inline std::string toString()
        {
            /*std::stringstream stream;
            stream << "id:" << id << " base:" << base << " quote:" << quote;*/
            return std::string("id:" + id + " base:" + base + " quote:" + quote);
        }
    };

class AbstractExchangeController
{
    public:
        virtual ~AbstractExchangeController(){};
        virtual CurrencyPair getNewCurrencyPairSync(const std::string& quote) const  =0;
        virtual TickerResult getSpotTicker(const std::string& currencyPair) const =0;
        virtual OrderResult sendOrder(const std::string& currency_pair, const Side side, double quantity, double price) const  =0;
};

} /* end ExchangeController namespace */ 