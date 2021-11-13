#include <optional>
#include <math.h> 
#include "NewListedCurrencyBot.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include "magic_enum.hpp"
#include "tools.hpp"
#include "PriceWatcher.hpp"

namespace Bot
{

NewListedCurrencyBot::NewListedCurrencyBot(const ExchangeController::AbstractExchangeController& exchangeController
    , const BotConfig& botConfig)
: _exchangeController(exchangeController)
, _pairId(botConfig.getPairId())
, _limitBuyPrice(botConfig.getLimitBuyPrice())
, _priceThresholdConfig(botConfig.getPriceThresholdConfig())
, _timeThreasholdConfig(botConfig.getTimeThresholdConfig())
{
    if(auto quantityOpt = botConfig.getQuantity())
        _quantity = *quantityOpt;
    else
    {
        const double percent = 0.97;
        _quantity = Quantity{floor((_exchangeController.computeMaxQuantity(_limitBuyPrice) * percent)/_limitBuyPrice)};
        LOG_INFO << "Quantity automatically computed to " << _quantity << " (" << percent << " of the max amount available)";
    }
    LOG_INFO << "Max position size: " << _quantity * _limitBuyPrice << " USDT";
}

NewListedCurrencyBot::~NewListedCurrencyBot()
{}

void NewListedCurrencyBot::run()
{
    // Wait
    std::optional<ExchangeController::OrderResult> buyOrderResult = buySync();
    if(!buyOrderResult)
        return;

    if(buyOrderResult->status != ExchangeController::OrderStatus::Closed)
    {
        LOG_ERROR << "Cannot buy " << _pairId << " because buyOrderResult.status=" << magic_enum::enum_name(buyOrderResult->status);
        return;
    }

    // Wait
    shouldSellSync(*buyOrderResult);
    ExchangeController::OrderResult sellOrderResult = sellAll(*buyOrderResult);

    if(sellOrderResult.status != ExchangeController::OrderStatus::Closed)
    {
        LOG_ERROR << "Cannot sell " << _pairId 
            << " because buyOrderResult.status=" << magic_enum::enum_name(sellOrderResult.status) 
            << " *** POSITION IS OPEN ***";
        return;
    }

    const double buyPrice = buyOrderResult->fillPrice / buyOrderResult->amount;
    const double sellPrice = sellOrderResult.fillPrice / sellOrderResult.amount;
    const double pnl = sellOrderResult.fillPrice - buyOrderResult->fillPrice - (buyOrderResult->fee * buyPrice) - (sellOrderResult.fee * sellPrice);
    LOG_INFO << "Pnl: " << pnl << " USDT, " << (pnl/buyOrderResult->fillPrice)*100 << "%."
        << " Buy: " << buyPrice << " USDT."
        << " Sell: " << sellPrice << " USDT."
        << " Amount invested: " << buyOrderResult->fillPrice << " USDT.";

}

std::optional<ExchangeController::OrderResult> NewListedCurrencyBot::buySync()
{
    ExchangeController::OrderResult buyOrderResult;
    do
    {
        CHRONO_THIS_SCOPE;
        if(tools::kbhit())
        {
            LOG_INFO << "key pressed, stopping...";
            return std::nullopt;
        }

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
    Bot::PriceWatcher priceWatcher(_timeThreasholdConfig);
    tools::LinearExtrapoler priceThreasholdExtrapoler(_priceThresholdConfig.lowBound, _priceThresholdConfig.highBound);

    while(!tools::kbhit())
    {
        const ExchangeController::TickerResult tickerResult = _exchangeController.getSpotTicker(_pairId);
        const double profit = tickerResult.high24h/purchasePrice;
        const double lossThreshold = priceThreasholdExtrapoler.extrapolate(profit);

        if(previousTickerResult != tickerResult)
        {
            LOG_INFO << "purchasePrice: " << purchasePrice
                    << " current PROFIT=" << ((tickerResult.last - purchasePrice)/purchasePrice)*100 << "%"
                    << " current lossThreshold=" << lossThreshold
                    << " tickerResult " << tickerResult.toString();
            //LOG_INFO << "OrderBook: " << _exchangeController.getOrderBook(_pairId);
            previousTickerResult = tickerResult;
        }

        if(tickerResult.high24h != 0 && tickerResult.last < (tickerResult.high24h * lossThreshold))
            return;

        if(!priceWatcher.isMoving(tickerResult.last, profit))
        {
            LOG_INFO << "Price doesn't move anymore, stopping... ";
            return;
        }
    }

    LOG_INFO << "key pressed, stopping...";
}

void NewListedCurrencyBot::watch() const
{
    ExchangeController::TickerResult previousTickerResult;
    Bot::PriceWatcher priceWatcher(_timeThreasholdConfig);
    tools::LinearExtrapoler priceThreasholdExtrapoler(_priceThresholdConfig.lowBound, _priceThresholdConfig.highBound);
    double profit = 1;
    while(!tools::kbhit())
    {
        const ExchangeController::TickerResult tickerResult = _exchangeController.getSpotTicker(_pairId);
        if(previousTickerResult != tickerResult)
        {
            LOG_INFO << "tickerResult " << tickerResult.toString();
            //LOG_INFO << "jsonOrderBook " << _exchangeController.getOrderBook(_pairId);
            previousTickerResult = tickerResult;
        }

        LOG_DEBUG << "profit=" << profit << " priceThreshold=" << priceThreasholdExtrapoler.extrapolate(profit);

        if(!priceWatcher.isMoving(tickerResult.last, profit))
        {
            LOG_INFO << "Price doesn't move anymore, stopping... ";
            return;
        }

        profit += 0.1;
    }
}

ExchangeController::OrderResult NewListedCurrencyBot::sellAll(const ExchangeController::OrderResult& buyOrderResult)
{
    if(buyOrderResult.amount)
    {
        double amountLeft = buyOrderResult.amount - buyOrderResult.fee;
        double smallPrice = buyOrderResult.fillPrice / buyOrderResult.amount * 0.2;

        // For small order smallPrice * amount can be lower than the exchange min order size and it fails
        if(smallPrice * amountLeft < _exchangeController.getMinOrderSize())
            smallPrice = (_exchangeController.getMinOrderSize() * 1.1) / amountLeft;

        return _exchangeController.sendOrder(_pairId, ExchangeController::Side::sell, amountLeft, smallPrice);
    }
    return ExchangeController::OrderResult();
}

} /* end namespace Bot */ 