#include "exchangeController/BinanceController.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include <optional>
#include "boost/lexical_cast.hpp"
#include "magic_enum.hpp"
#include <chrono>
#include <thread>


namespace {

BinaCPP::Side convertFrom(ExchangeController::Side side)
{
    switch (side)
    {
    case ExchangeController::Side::buy: return BinaCPP::Side::BUY;
    case ExchangeController::Side::sell: return BinaCPP::Side::SELL;
    default:
        throw ExchangeController::ExchangeControllerException("Define " + std::string(magic_enum::enum_name(side)) + "here: BinaCPP::Side convertFrom(ExchangeController::Side side)");
    }
}

ExchangeController::OrderStatus fillOrderStatus(const Json::Value& result)
{
    // error Json contain message field
    if(const auto message = result.get("code", Json::Value()); !message.empty())
    {
        if(message.toStyledString().find("1121", 0) != std::string::npos)
            return ExchangeController::OrderStatus::CurrencyNotAvailable; // invalid currency
        if(message.toStyledString().find("1013", 0) != std::string::npos)
            return ExchangeController::OrderStatus::SizeTooSmall;
        if(message.toStyledString().find("2010", 0) != std::string::npos)
            return ExchangeController::OrderStatus::NotEnoughBalance;
        // order cancel if limit price too high ??
    }
    else if(const auto message = result.get("status", Json::Value()); !message.empty())
    {
        if(message.toStyledString().find("FILLED", 0) != std::string::npos
            || message.toStyledString().find("PARTIALLY_FILLED", 0) != std::string::npos )
            return ExchangeController::OrderStatus::Closed;
        if(message.toStyledString().find("NEW", 0) != std::string::npos)
            return ExchangeController::OrderStatus::New;

        if(message.toStyledString().find("CANCELED", 0) != std::string::npos
            || message.toStyledString().find("PENDING_CANCEL", 0) != std::string::npos
            || message.toStyledString().find("REJECTED", 0) != std::string::npos
            || message.toStyledString().find("EXPIRED", 0) != std::string::npos)
            return ExchangeController::OrderStatus::Cancelled;
    }

    return ExchangeController::OrderStatus::Unknown;
}
}

namespace ExchangeController
{

BinanceController::BinanceController(const Bot::ApiKeys& apiKeys)
: _binanceAPI(apiKeys.pub, apiKeys.secret)
, _binanceAPI2(apiKeys.pub, apiKeys.secret)
{}

BinanceController::~BinanceController()
{
}

TickerResult BinanceController::getSpotTicker(const std::string& currencyPair) const
{
    //CHRONO_THIS_SCOPE;
    static size_t failureCount = 0;
    static double high_24h = 0; // artificial because not suported by the api but required
    Json::Value result;

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    _binanceAPI2.get24Ticker(currencyPair, result);

    //LOG_INFO << result;

    if(result.get("lastPrice", Json::Value()).empty())
    {
        if(++failureCount > 10)
            throw ExchangeControllerException("BinanceController::getSpotTicker cannot get Ticker *** POSITION IS OPEN ***");
        
        LOG_ERROR << "Issue with ticker: " << result;
        sleep(1);
        return {};
    }
    failureCount = 0;

    double last = boost::lexical_cast<double>(result["lastPrice"].asString());
    double high24 = boost::lexical_cast<double>(result["highPrice"].asString());

    if(high24 == 0)
    {
        LOG_WARNING << "high24 equal 0";
        if(last > high_24h)
            high_24h = last;
        
        high24 = high_24h;
    }

    return { .last = last
        , .high24h = high24
        , .low24h = boost::lexical_cast<double>(result["lowPrice"].asString())
        , .baseVolume = boost::lexical_cast<double>(result["volume"].asString())
        , .quoteVolume = boost::lexical_cast<double>(result["quoteVolume"].asString())
        , .lowestAsk = boost::lexical_cast<double>(result["askPrice"].asString())
        , .highestBid = boost::lexical_cast<double>(result["bidPrice"].asString())};
}

std::string BinanceController::getOrderBook(const std::string& ) const
{
    return "";
}

//Response: {
//        "clientOrderId" : "3OyxLW0qcH7R5pUQAumFtk",
//        "cummulativeQuoteQty" : "69.45290000",
//        "executedQty" : "3.68000000",
//        "fills" : 
//        [
//                {
//                        "commission" : "0.02133440",
//                        "commissionAsset" : "USDT",
//                        "price" : "18.88000000",
//                        "qty" : "1.13000000",
//                        "tradeId" : 768743
//                },
//                {
//                        "commission" : "0.04811850",
//                        "commissionAsset" : "USDT",
//                        "price" : "18.87000000",
//                        "qty" : "2.55000000",
//                        "tradeId" : 768744
//                }
//        ],
//        "orderId" : 5050124,
//        "orderListId" : -1,
//        "origQty" : "3.68000000",
//        "price" : "0.00000000",
//        "side" : "SELL",
//        "status" : "FILLED",
//        "symbol" : "ANYUSDT",
//        "timeInForce" : "GTC",
//        "transactTime" : 1639711101265,
//        "type" : "MARKET"
//}
OrderResult BinanceController::sendOrder(const std::string& currencyPair, const Side side, const Quantity& quantity, const Price& price) const
{
    Json::Value result, resultOrderInfo, resultOrderBook;
    _binanceAPI.send_order(currencyPair, convertFrom(side), BinaCPP::Type::MARKET, BinaCPP::TimeInForce::IOC, quantity, price, result);

    const auto& status = fillOrderStatus(result);

    LOG_INFO << "Response: " << result;

    if(status == OrderStatus::Closed || status == OrderStatus::Cancelled)
    {
        Quantity executedQty, fillPrice, commission;
        executedQty = Quantity(result["executedQty"].asString());
        for(const auto& fill: result["fills"])
        {
            commission = commission + Quantity(fill["commission"].asString()); //TODO += operator to define
            fillPrice = fillPrice + Quantity(fill["price"].asString()) * Quantity(fill["qty"].asString());
        }

        return { .status = status
        , .fillPrice = fillPrice
        , .filledTotal = fillPrice
        , .amount = executedQty
        , .fee = commission };
    }
    else
        return {status, Quantity(), Quantity(), Quantity(), Quantity()};
    
}

Quantity BinanceController::computeMaxAmount(const Price& price) const
{
    /*Json::Value result;
    _binanceAPI.getAccountBalances(result);
    for(const auto& account : result["data"])
    {
        if(account["currency"] == "USDT")
        {
            Quantity quantity(account["available"].asString());
            const tools::FixedPoint percent("0.97");

            return Quantity{(quantity * percent)};
        }
    }
    LOG_DEBUG << result;*/
    return Quantity("70");
    return Quantity();
}

Quantity BinanceController::prepareAccount(const Price& price,const std::optional<Quantity>& maxAmount, const std::optional<Quantity>& quantity) const
{
    return computeMaxAmount(price); // account transfer to do
}

Quantity BinanceController::getMinOrderSize() const
{
    return Quantity{"70"}; // 0.001 BTC ..
}

Quantity BinanceController::getAmountLeft(const OrderResult& buyOrderResult) const
{
    return buyOrderResult.amount - buyOrderResult.fee;
}

}