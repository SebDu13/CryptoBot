#pragma once
#include <string>

namespace ExchangeController
{
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
    };

class AbastractExchangeController
{
    public:
        virtual ~AbastractExchangeController(){};
        virtual CurrencyPair getNewCurrencyPairSync() const =0;
        virtual TickerResult getSpotTickerSync(std::string& currency_pair) const =0;
        virtual OrderResult sendOrder(std::string& currency_pair, const Side side, double quantity, double price) const  =0;
};

} /* end ExchangeController namespace */ 