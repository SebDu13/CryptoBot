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

    struct OrderResult
    {
        double price;
        double amount;
    };

    struct TickerResult
    {
        std::string last;
        std::string high24h;
        std::string low24h;
        std::string baseVolume;
        std::string quoteVolume;
        std::string lowestAsk;
        std::string highestBid;
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
    };

class AbastractExchangeController
{
    public:
        virtual ~AbastractExchangeController(){};
        virtual CurrencyPair getNewCurrencyPairSync() =0;
        virtual TickerResult getSpotTicker(const std::string& currencyPair) const =0;
        virtual OrderResult sendOrder(const std::string& currency_pair, const Side side, size_t quantity, const std::string& price) const  =0;
};

} /* end ExchangeController namespace */ 