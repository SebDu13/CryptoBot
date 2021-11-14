#include "exchangeController/KucoinController.hpp"
#include "chrono.hpp"
#include "logger.hpp"
#include <optional>
#include "boost/lexical_cast.hpp"
#include "magic_enum.hpp"

namespace {

KucoinCPP::Side convertFrom(ExchangeController::Side side)
{
    switch (side)
    {
    case ExchangeController::Side::buy: return KucoinCPP::Side::buy;
    case ExchangeController::Side::sell: return KucoinCPP::Side::sell;
    default:
        throw ExchangeController::ExchangeControllerException("Define " + std::string(magic_enum::enum_name(side)) + "here: GateIoCPP::Side convertFrom(ExchangeController::Side side)");
    }
}

ExchangeController::OrderStatus fillOrderStatus(const Json::Value& result)
{
    // error Json contain message field
    if(const auto message = result.get("code", Json::Value()); !message.empty())
    {
        if(message.toStyledString().find("200000", 0) != std::string::npos)
            return ExchangeController::OrderStatus::Closed;
        if(message.toStyledString().find("900001", 0) != std::string::npos)
            return ExchangeController::OrderStatus::InvalidCurrency;
        if(message.toStyledString().find("400100", 0) != std::string::npos)
            return ExchangeController::OrderStatus::SizeTooSmall;
        if(message.toStyledString().find("200004", 0) != std::string::npos)
            return ExchangeController::OrderStatus::NotEnoughBalance;
        // order cancel if limit price too high ??
    }
    return ExchangeController::OrderStatus::Unknown;
}
}

namespace ExchangeController
{

KucoinController::KucoinController(const Bot::ApiKeys& apiKeys)
: _kucoinAPI(apiKeys.pub, apiKeys.secret, apiKeys.passphrase)
{}

KucoinController::~KucoinController()
{
}

TickerResult KucoinController::getSpotTicker(const std::string& currencyPair) const
{
    //CHRONO_THIS_SCOPE;
    static double high_24h = 0; // artificial because not suported by the api but required
    Json::Value result;
    _kucoinAPI.getTicker(currencyPair, result);

    double last = boost::lexical_cast<double>(result[0]["price"].asString());

    if(last > high_24h)
        high_24h = last;

    return { .last = last
        , .high24h = high_24h
        , .low24h = 0
        , .baseVolume = 0
        , .quoteVolume = 0
        , .lowestAsk = boost::lexical_cast<double>(result[0]["bestAsk"].asString())
        , .highestBid = boost::lexical_cast<double>(result[0]["bestBid"].asString())};
}

std::string KucoinController::getOrderBook(const std::string& ) const
{
    return "";
}

OrderResult KucoinController::sendOrder(const std::string& currencyPair, const Side side, const Quantity& quantity, const Price& price) const
{
    Json::Value result, resultOrderInfo;
    _kucoinAPI.sendLimitOrder(currencyPair, convertFrom(side), KucoinCPP::TimeInForce::IOC, quantity, price, result);

    const auto& status = fillOrderStatus(result);

    LOG_DEBUG << result;

    if(status != OrderStatus::Closed)
        return {status, Quantity(), Quantity(), Quantity(), Quantity()};

    _kucoinAPI.getOrder(result["data"]["orderId"].asString(), resultOrderInfo);
    LOG_DEBUG << resultOrderInfo;

    if(resultOrderInfo["code"].asString() != "200000")
        throw ExchangeControllerException("KucoinController::sendOrder cannot get order info *** POSITION IS OPEN ***");

    const auto& data = resultOrderInfo["data"];
    return { (data["cancelExist"].asString() == "true" ? OrderStatus::Cancelled: status)
    , Quantity(data["dealFunds"].asString())
    , Quantity(data["dealFunds"].asString())
    , Quantity(data["dealSize"].asString())
    , Quantity(data["fee"].asString()) };
}

    Quantity KucoinController::computeMaxQuantity(const Price& price) const
    {
        return Quantity();
    }

    Quantity KucoinController::getMinOrderSize() const
    {
        return Quantity();
    }


}