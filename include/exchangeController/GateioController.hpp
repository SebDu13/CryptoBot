#pragma once

#include "gateiocpp.h"
#include "exchangeController/AbstractExchangeController.hpp"
#include <string>
#include <unordered_set>

namespace ExchangeController{

class GateioController: public AbastractExchangeController
{
    public:
        GateioController(std::string &api_key, std::string &secret_key);
        virtual ~GateioController();
        CurrencyPair getNewCurrencyPairSync() override;
        TickerResult getSpotTicker(const std::string& currencyPair) const override;
        OrderResult sendOrder(const std::string& currency_pair, const Side side, size_t quantity, const std::string& price) const override;

    private:
    GateIoCPP gateIoAPI;
    std::unordered_set<std::string> allCurrencyPairsCache;
    std::size_t rawCurrencyPairsResultSize;

    std::optional<CurrencyPair> findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result) const;
};

} /* end ExchangeController namespace */ 