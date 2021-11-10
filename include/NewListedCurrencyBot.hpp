#pragma once
#include <string>
#include "exchangeController/AbstractExchangeController.hpp"
#include "BotType.hpp"
#include "BotConfig.hpp"
#include "ThresholdService.hpp"

/* TODO:
1- récuperer l'heure de lancement grace à une vente limite qui rate et qui donne:
    label" : "INVALID_CURRENCY",
    "message" : "PIZA_USDT trade is disabled until 2021-11-05T18:00+08:00[Asia/Shanghai]"
*/

namespace Bot
{

class NewListedCurrencyBot
{
    public:
    NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
        , const BotConfig& botconfig
        , ThresholdService thresholdService);
        
    virtual ~NewListedCurrencyBot();
    void run();
    void watch() const;
    void runWithoutMonitoring(const std::string& pairId);

    private:    
    const ExchangeController::AbstractExchangeController& _exchangeController;
    ThresholdService _thresholdService;
    const std::string _pairId;
    Price _limitBuyPrice;
    Quantity _quantity;

    void shouldSellSync(const ExchangeController::OrderResult& buyOrderResult) const;
    ExchangeController::OrderResult sellAll(const ExchangeController::OrderResult& buyOrderResult);
    std::optional<ExchangeController::OrderResult> buySync();
    double getSmallPrice(double price) const;
};

} /* end namespace Bot */ 