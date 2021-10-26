#pragma once
#include <string>
#include <unordered_set>
#include "exchangeController/AbstractExchangeController.hpp"
#include "gateiocpp.h"

namespace ExchangeController{

class GateioController: public AbastractExchangeController
{
    public:
        GateioController(std::string &api_key, std::string &secret_key);
        virtual ~GateioController();
        CurrencyPair getNewCurrencyPairSync() const override;
        TickerResult getSpotTickerSync(std::string& currency_pair) const override;
        OrderResult sendOrder(std::string& currency_pair, const Side side, double quantity, double price) const override;

    private:
    GateIoCPP gateIoAPI;
    std::unordered_set<std::string> allCurrencyPairsCache;
};

} /* end ExchangeController namespace */ 