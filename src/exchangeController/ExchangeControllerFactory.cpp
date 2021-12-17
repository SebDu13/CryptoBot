#include "exchangeController/ExchangeControllerFactory.hpp"
#include "exchangeController/GateioController.hpp"
#include "exchangeController/KucoinController.hpp"
#include "exchangeController/BinanceController.hpp"
#include "logger.hpp"
#include "magic_enum.hpp"

namespace ExchangeController
{

std::unique_ptr<AbstractExchangeController> ExchangeControllerFactory::create(const Bot::BotConfig& config)
{
    const auto exchange = config.getExchange();
    switch(exchange)
    {
        case Bot::Exchange::Gateio:
            return std::make_unique<ExchangeController::GateioController> (config.getApiKeys());
        case Bot::Exchange::Kucoin:
            return std::make_unique<ExchangeController::KucoinController> (config.getApiKeys());
        case Bot::Exchange::Binance:
            return std::make_unique<ExchangeController::BinanceController> (config.getApiKeys());
        default:
            LOG_ERROR << magic_enum::enum_name(exchange) << " not supported";
        break;
    }
    return nullptr;
}


}