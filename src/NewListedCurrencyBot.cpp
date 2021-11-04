#include "NewListedCurrencyBot.hpp"
#include "chrono.hpp"
#include "logger.hpp"

namespace Bot
{

NewListedCurrencyBot::NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController, AllocatedMoney allocatedMoney)
: exchangeController(exchangeController)
, allocatedMoney(allocatedMoney)
{
}

NewListedCurrencyBot::~NewListedCurrencyBot()
{}

void NewListedCurrencyBot::run()
{
    // blocking function
    ExchangeController::CurrencyPair newPair = exchangeController.getNewCurrencyPairSync(allocatedMoney.currency);
    LOG_DEBUG << "New currency pair found " << newPair.toString();

    ExchangeController::TickerResult firstSpotTicker= exchangeController.getSpotTicker(newPair.id);

}

} /* end namespace Bot */ 