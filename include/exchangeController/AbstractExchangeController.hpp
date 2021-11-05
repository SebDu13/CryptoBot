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
        OrderSizeTooLarge,
        InvalidCurrency,
        Unknown
    };

    struct OrderResult
    {
        OrderStatus status = OrderStatus::Unknown;
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

        inline std::string toString()
        {
            return std::string("last:" + std::to_string(last)
            + " high24h:" + std::to_string(high24h)
            + " low24h:" + std::to_string(low24h)
            + " baseVolume:" + std::to_string(baseVolume)
            + " quoteVolume:" + std::to_string(quoteVolume)
            + " lowestAsk:" + std::to_string(lowestAsk)
            + " highestBid:" + std::to_string(highestBid));
        }
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