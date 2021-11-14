#include "exchangeController/ExchangeControllerFactory.hpp"
#include "exchangeController/GateioController.hpp"
#include "exchangeController/KucoinController.hpp"
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
            return std::make_unique<ExchangeController::GateioController> (config.getApiKeys(exchange));
        case Bot::Exchange::Kucoin:
            return std::make_unique<ExchangeController::KucoinController> (config.getApiKeys(exchange));
    }
    LOG_ERROR << magic_enum::enum_name(exchange) << " not supported";
    return nullptr;
}


}