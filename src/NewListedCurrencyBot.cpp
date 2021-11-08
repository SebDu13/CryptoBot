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
        , ThresholdService thresholdService)
: _exchangeController(exchangeController)
, _pairId(pairId)
, _limitBuyPrice(limitBuyPrice)
, _quantity(quantity)
, _thresholdService(thresholdService)
{
}

NewListedCurrencyBot::~NewListedCurrencyBot()
{}

void NewListedCurrencyBot::run()
{
    // Wait
    ExchangeController::OrderResult buyOrderResult = buySync();

    if(buyOrderResult.status != ExchangeController::OrderStatus::Closed)
    {
        LOG_ERROR << "Cannot buy " << _pairId << " because buyOrderResult.status=" << magic_enum::enum_name(buyOrderResult.status);
        return;
    }

    // Wait
    shouldSellSync(buyOrderResult);
    ExchangeController::OrderResult sellOrderResult = sellAll(buyOrderResult);

    if(sellOrderResult.status != ExchangeController::OrderStatus::Closed)
    {
        LOG_ERROR << "Cannot sell " << _pairId 
            << " because buyOrderResult.status=" << magic_enum::enum_name(sellOrderResult.status) 
            << "*** POSITION IS OPEN ***";
        return;
    }

    const double pnl = sellOrderResult.fillPrice - sellOrderResult.fee - buyOrderResult.fillPrice - buyOrderResult.fee;
    LOG_INFO << "Pnl: " << pnl << " USDT. " << (pnl/buyOrderResult.fillPrice)*100 << "%";

}

ExchangeController::OrderResult NewListedCurrencyBot::buySync()
{
    ExchangeController::OrderResult buyOrderResult;
    do
    {
        CHRONO_THIS_SCOPE;
        buyOrderResult = _exchangeController.sendOrder(_pairId, ExchangeController::Side::buy , _quantity, _limitBuyPrice);

        if(buyOrderResult.status != ExchangeController::OrderStatus::InvalidCurrency)
            LOG_DEBUG << magic_enum::enum_name(buyOrderResult.status);

    } while (buyOrderResult.status == ExchangeController::OrderStatus::InvalidCurrency);

    return buyOrderResult;
}


void NewListedCurrencyBot::shouldSellSync(const ExchangeController::OrderResult& buyOrderResult) const
{
    ExchangeController::TickerResult previousTickerResult;
    const double purchasePrice = buyOrderResult.fillPrice / buyOrderResult.amount;
    while(true)
    {
        const ExchangeController::TickerResult tickerResult = _exchangeController.getSpotTicker(_pairId);
        const double gain = tickerResult.high24h/purchasePrice;
        const double lossThreshold = _thresholdService.getLossThreshold(gain);

        if(previousTickerResult != tickerResult)
        {
            LOG_INFO << "purchasePrice: " << purchasePrice
                    << " current gain=" << gain
                    << " current lossThreshold=" << lossThreshold
                    << " tickerResult " << tickerResult.toString();

            previousTickerResult = tickerResult;
        }

        if(tickerResult.high24h != 0 && tickerResult.last < (tickerResult.high24h * lossThreshold))
            return;
    }

    // faire un algo qui plus high24 est élevé du prix d'entrée, plus la limit de vente sera proche ?
}

void NewListedCurrencyBot::watch() const
{
    ExchangeController::TickerResult previousTickerResult;
    while(true)
    {
        const ExchangeController::TickerResult tickerResult = _exchangeController.getSpotTicker(_pairId);
        if(previousTickerResult != tickerResult)
        {
            LOG_INFO << "tickerResult " << tickerResult.toString();
            previousTickerResult = tickerResult;
        }
    }

    // faire un algo qui plus high24 est élevé du prix d'entrée, plus la limit de vente sera proche ?
}

ExchangeController::OrderResult NewListedCurrencyBot::sellAll(const ExchangeController::OrderResult& buyOrderResult)
{
    if(buyOrderResult.amount)
    {
        double amountLeft = buyOrderResult.amount - buyOrderResult.fee;
        double sellPrice = (buyOrderResult.fillPrice / buyOrderResult.amount)*0.2; // 20% of the price we bought then we are sure to be executed
        return _exchangeController.sendOrder(_pairId, ExchangeController::Side::sell, amountLeft, sellPrice);
    }
    return ExchangeController::OrderResult();
}

} /* end namespace Bot */ 