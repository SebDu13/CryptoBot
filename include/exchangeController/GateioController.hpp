#pragma once

#include <string>
#include <unordered_set>
#include "gateiocpp.h"
#include "exchangeController/AbstractExchangeController.hpp"

namespace ExchangeController{

class GateioController: public AbstractExchangeController
{
    public:
    static inline const double MIN_ORDER_PRICE =  0.0001;

    GateioController(const Bot::ApiKeys& apiKeys);
    virtual ~GateioController();
    CurrencyPair getNewCurrencyPairSync(const std::string& quote) const override;
    TickerResult getSpotTicker(const std::string& currencyPair) const override;
    std::string getOrderBook(const std::string& currencyPair) const override;
    OrderResult sendOrder(const std::string& currencyPair, const Side side, double quantity, double price) const override;
    double getMinPrice() const override;
    Bot::Quantity computeMaxQuantity(double price) const override;

    private:
    GateIoCPP gateIoAPI;
    std::unordered_set<std::string> allCurrencyPairsCache;
    std::size_t rawCurrencyPairsResultSize;

    std::optional<CurrencyPair> findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result, const std::string& quote) const;
};

} /* end ExchangeController namespace */ 