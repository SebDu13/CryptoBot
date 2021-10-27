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
        TickerResult getSpotTicker(std::string& currency_pair) const override;
        OrderResult sendOrder(std::string& currency_pair, const Side side, double quantity, double price) const override;

    private:
    GateIoCPP gateIoAPI;
    std::unordered_set<std::string> allCurrencyPairsCache;

    CurrencyPair findNewPairFrom(const GateIoCPP::CurrencyPairsResult& result) const;
};

} /* end ExchangeController namespace */ 