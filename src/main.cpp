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
#include "exchangeController/GateioController.hpp"
#include "magic_enum.hpp"
#include "NewListedCurrencyBot.hpp"
#include "BotType.hpp"
#include "BotConfig.hpp"
#include "tools.hpp"


int main(int argc, char **argv) 
{
	using namespace Bot;

	BotConfig botConfig;
	if(botConfig.loadOptionsFromMain(argc, argv) == Status::Failure)
		return -1;

	Logger::init(Logger::FilterLevel::Debug, botConfig.getPairId(), botConfig.getWithConsole());
	LOG_INFO << botConfig.toString();

	// *** KUCOIN ***
	/*Json::Value result24hr;
	Json::Value resultTicker;
	KucoinCPP kucoinCPP("","");
	int i = 10;
	while(true)
	{
		kucoinCPP.getTicker(botConfig.getPairId(), resultTicker);
		LOG_DEBUG << "getTicker " << resultTicker;
		kucoinCPP.get24HrStats(botConfig.getPairId(), resultTicker);
		LOG_DEBUG << "result24hr " << resultTicker;
	}*/
	// ***

	// *** GATEIO ***
	ExchangeController::GateioController gateioController{botConfig.getApiKeys()};
	Bot::NewListedCurrencyBot newListedCurrencyBot(
		gateioController
		, botConfig);

	newListedCurrencyBot.run();
	//newListedCurrencyBot.watch();

	return 0;	
}