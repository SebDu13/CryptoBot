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
	HuobiApi huobiApi("7af2f75c-2ffb0b7f-d4bdd157-3d2xc4v5bu","5d52900a-23c75337-b3241238-683fa","");
	//huobiApi.getTicker(botConfig.getPairId(), result);
	huobiApi.getAccounts(result);
	LOG_INFO << result;



	ExchangeController::HuobiController huobiController(Bot::ApiKeys{"",""});
	auto tickerResult = huobiController.getSpotTicker(botConfig.getPairId());
	LOG_INFO << tickerResult.toString();*/


	/*auto kucoinController = ExchangeController::ExchangeControllerFactory::create(botConfig);
	kucoinController->sendOrder(botConfig.getPairId(), ExchangeController::Side::buy, Quantity("10"), botConfig.getLimitBuyPrice());*/

	
	return 0;
}