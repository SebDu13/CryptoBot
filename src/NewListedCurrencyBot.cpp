#include "NewListedCurrencyBot.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include "magic_enum.hpp"

namespace Bot
{

NewListedCurrencyBot::NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
        , std::string pairId
        , double limitBuyPrice
        , double quantity
        , double lossThresholdPercent)
: _exchangeController(exchangeController)
, _pairId(pairId)
, _limitBuyPrice(limitBuyPrice)
, _quantity(quantity)
, _lossThresholdPercent(lossThresholdPercent)
{
}

NewListedCurrencyBot::NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
        , std::string pairId
        , double limitBuyPrice
        , double quantity)
: _exchangeController(exchangeController)
, _pairId(pairId)
, _limitBuyPrice(limitBuyPrice)
, _quantity(quantity)
{
}

NewListedCurrencyBot::~NewListedCurrencyBot()
{}

void NewListedCurrencyBot::run()
{
    ExchangeController::OrderResult buyOrderResult;
    do
    {
        buyOrderResult = _exchangeController.sendOrder(_pairId, ExchangeController::Side::buy , _quantity, _limitBuyPrice);
    } while (buyOrderResult.status == ExchangeController::OrderStatus::InvalidCurrency);
    
    if(buyOrderResult.status == ExchangeController::OrderStatus::Closed)
    {
        // Wait
        shouldSellSync(buyOrderResult);

        // minimum order size has to be 1USDT
        ExchangeController::OrderResult sellOrderResult = _exchangeController.sendOrder(_pairId, ExchangeController::Side::sell , _quantity, 1.01/_quantity);
    }
    else
        LOG_ERROR << "Could not buy " << _pairId << " because buyOrderResult.status=" << magic_enum::enum_name(buyOrderResult.status);
}

void NewListedCurrencyBot::shouldSellSync(const ExchangeController::OrderResult& buyOrderResult) const
{
    double limitPrice = (buyOrderResult.fillPrice / buyOrderResult.amount) * _lossThresholdPercent;
    LOG_INFO << "limitPrice: " << limitPrice;
    while(true)
    {
        ExchangeController::TickerResult tickerResult = _exchangeController.getSpotTicker(_pairId);
        LOG_DEBUG << " tickerResult " << tickerResult.toString();
        if(tickerResult.last < limitPrice)
            return;
    }
}

} /* end namespace Bot */ 