#pragma once
#include <string>
#include "exchangeController/AbstractExchangeController.hpp"

namespace Bot
{

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
    const double _lossThresholdPercent = 0.8;

    void shouldSellSync(const ExchangeController::OrderResult& buyOrderResult) const;
};

} /* end namespace Bot */ 