#pragma once
#include <string>
#include <memory>
#include "exchangeController/AbstractExchangeController.hpp"
#include "BotType.hpp"
#include "BotConfig.hpp"
#include "LinearExtrapoler.hpp"

/* TODO:
1- récuperer l'heure de lancement grace à une vente limite qui rate et qui donne:
    label" : "INVALID_CURRENCY",
    "message" : "PIZA_USDT trade is disabled until 2021-11-05T18:00+08:00[Asia/Shanghai]"
*/

namespace Bot
{

class ListingBot
{
    public:
    ListingBot(std::unique_ptr<ExchangeController::AbstractExchangeController> exchangeController
        , const BotConfig& botconfig);
        
    virtual ~ListingBot();
    void run();
    void watch() const;
    void runWithoutMonitoring(const std::string& pairId);

    private:    
    std::unique_ptr<ExchangeController::AbstractExchangeController> _exchangeController;
    const std::string _pairId;
    Price _limitBuyPrice;
    Quantity _quantity;
    PriceThresholdConfig _priceThresholdConfig;
    TimeThresholdConfig _timeThreasholdConfig;

    void shouldSellSync(const ExchangeController::OrderResult& buyOrderResult) const;
    ExchangeController::OrderResult sellAll(const ExchangeController::OrderResult& buyOrderResult);
    std::optional<ExchangeController::OrderResult> buySync();
};

} /* end namespace Bot */ 