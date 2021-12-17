#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <json/json.h>
#include <unistd.h>
#include <chrono>
#include <thread>

#include "logger.hpp"
#include "binacpp.h"
#include "kucoincpp.hpp"
#include "HuobiApi.hpp"
#include "exchangeController/GateioController.hpp"
#include "exchangeController/KucoinController.hpp"
#include "exchangeController/HuobiController.hpp"
#include "exchangeController/ExchangeControllerFactory.hpp"
#include "exchangeController/AbstractExchangeController.hpp"
#include "magic_enum.hpp"
#include "ListingBot.hpp"
#include "BotType.hpp"
#include "BotConfig.hpp"
#include "BotManager.hpp"
#include "tools.hpp"

#include <boost/multiprecision/cpp_dec_float.hpp>


int main(int argc, char **argv) 
{
	using namespace Bot;

	BotConfig botConfig;
	if(botConfig.loadOptionsFromMain(argc, argv) == Status::Failure)
		return -1;

	Logger::init(Logger::FilterLevel::Debug
		, botConfig.getPairId()
		, botConfig.getWithConsole()
		, std::string(magic_enum::enum_name(botConfig.getExchange())));

	LOG_INFO << botConfig.toString();

	Bot::BotManager botmanager(botConfig);
	botmanager.startOnTime();

	/* *** HUOBI *** */
	/*Json::Value result;
	HuobiApi huobiApi("","","");
	//huobiApi.getTicker(botConfig.getPairId(), result);
	huobiApi.getAccounts(result);
	LOG_INFO << result;

	ExchangeController::HuobiController huobiController(Bot::ApiKeys{"",""});
	auto tickerResult = huobiController.getSpotTicker(botConfig.getPairId());
	LOG_INFO << tickerResult.toString();*/

	/* *** BINANCE *** */
	/*Json::Value result;
	auto key = botConfig.getApiKeys();
	BinaCPP _binanceAPI(key.pub, key.secret);
	_binanceAPI.send_order(botConfig.getPairId()
		, BinaCPP::Side::BUY
		, BinaCPP::Type::LIMIT
		, BinaCPP::TimeInForce::IOC
		, Quantity("10")
		, botConfig.getLimitBuyPrice()
		, result);
	LOG_INFO << result;

	auto binanceController = ExchangeController::ExchangeControllerFactory::create(botConfig);
	ExchangeController::OrderResult orderResult = binanceController->sendOrder(botConfig.getPairId(), ExchangeController::Side::buy, Quantity("10"), botConfig.getLimitBuyPrice());
	LOG_INFO << orderResult.toString();*/

	
	return 0;
}