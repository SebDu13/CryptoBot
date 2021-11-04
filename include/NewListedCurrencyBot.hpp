#pragma once
#include <string>
#include "exchangeController/AbstractExchangeController.hpp"

namespace Bot
{

struct AllocatedMoney
{
    double amount;
    std::string currency;
};

class NewListedCurrencyBot
{
    public:
    NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
        , AllocatedMoney allocatedMoney);
        
    virtual ~NewListedCurrencyBot();
    void run();

    private:    
    const ExchangeController::AbstractExchangeController& exchangeController;
    const AllocatedMoney allocatedMoney;
};

} /* end namespace Bot */ 