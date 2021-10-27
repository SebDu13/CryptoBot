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
    };

class AbastractExchangeController
{
    public:
        virtual ~AbastractExchangeController(){};
        virtual CurrencyPair getNewCurrencyPairSync() =0;
        virtual TickerResult getSpotTicker(std::string& currency_pair) const =0;
        virtual OrderResult sendOrder(std::string& currency_pair, const Side side, double quantity, double price) const  =0;
};

} /* end ExchangeController namespace */ 