#include "NewListedCurrencyBot.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include "magic_enum.hpp"

namespace Bot
{

NewListedCurrencyBot::NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
        , std::string pairId
        , Price limitBuyPrice
        , Quantity quantity
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
        , Price limitBuyPrice
        , Quantity quantity)
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
        CHRONO_THIS_SCOPE;
        buyOrderResult = _exchangeController.sendOrder(_pairId, ExchangeController::Side::buy , _quantity, _limitBuyPrice);

        if(buyOrderResult.status != ExchangeController::OrderStatus::InvalidCurrency)
            LOG_DEBUG << magic_enum::enum_name(buyOrderResult.status);

    } while (buyOrderResult.status == ExchangeController::OrderStatus::InvalidCurrency);
    
    if(buyOrderResult.status == ExchangeController::OrderStatus::Closed)
    {
        // Wait
        shouldSellSync(buyOrderResult);
        sellAll(buyOrderResult);
    }
    else
        LOG_ERROR << "Could not buy " << _pairId << " because buyOrderResult.status=" << magic_enum::enum_name(buyOrderResult.status);
}

void NewListedCurrencyBot::shouldSellSync(const ExchangeController::OrderResult& buyOrderResult) const
{
    while(true)
    {
        const ExchangeController::TickerResult tickerResult = _exchangeController.getSpotTicker(_pairId);
        LOG_DEBUG << "buyPrice: " << (buyOrderResult.fillPrice / buyOrderResult.amount) << " tickerResult " << tickerResult.toString();

        if(tickerResult.high24h != 0 && tickerResult.last < (tickerResult.high24h * _lossThresholdPercent))
            return;
    }

    // faire un algo qui plus high24 est élevé du prix d'entrée, plus la limit de vente sera proche ?
}

void NewListedCurrencyBot::sellAll(const ExchangeController::OrderResult& buyOrderResult)
{
    if(buyOrderResult.amount)
    {
        double amountLeft = buyOrderResult.amount - buyOrderResult.fee;
        double sellPrice = (buyOrderResult.fillPrice / buyOrderResult.amount)*0.2; // 20% of the price we bought then we are sure to be executed
            ExchangeController::OrderResult sellOrderResult = _exchangeController.sendOrder(_pairId, ExchangeController::Side::sell 
                                                                                    , amountLeft
                                                                                    , sellPrice);
    }
}

} /* end namespace Bot */ 