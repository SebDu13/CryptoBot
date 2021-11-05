#pragma once
#include <string>
#include "exchangeController/AbstractExchangeController.hpp"

/* TODO:
1- récuperer l'heure de lancement grace à une vente limite qui rate et qui donne:
    label" : "INVALID_CURRENCY",
    "message" : "PIZA_USDT trade is disabled until 2021-11-05T18:00+08:00[Asia/Shanghai]"
2- récuperer le prix de départ grace à l'order book qui fonctionne 30min avant le listing
3- stratégie de sortie variable, qui se resserre de plus en plus lorsque que le high24 monte
4- envoyer plusieurs ordres de vente pour etre sur de tout larguer
*/

namespace Bot
{

struct Price
{
    Price(double price):value(price){};
    double value;
};

class NewListedCurrencyBot
{
    public:
    NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
        , std::string pairId
        , double limitBuyPrice
        , double quantity
        , double lossThresholdPercent);

    NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
        , std::string pairId
        , double limitBuyPrice
        , double quantity);
        
    virtual ~NewListedCurrencyBot();
    void run();
    void runWithoutMonitoring(const std::string& pairId);

    private:    
    const ExchangeController::AbstractExchangeController& _exchangeController;
    const std::string _pairId;
    double _limitBuyPrice;
    double _quantity;
    const double _lossThresholdPercent = 0.95;

    void shouldSellSync(const ExchangeController::OrderResult& buyOrderResult) const;
    void sellAll(const ExchangeController::OrderResult& buyOrderResult);
};

} /* end namespace Bot */ 