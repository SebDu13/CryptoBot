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
#include "ThresholdService.hpp"


int main(int argc, char **argv) 
{
	using namespace Bot;

	BotConfig botConfig;
	if(botConfig.loadOptionsFromMain(argc, argv) == Status::Failure)
		return -1;

	Logger::init(Logger::FilterLevel::Debug, botConfig.getPairId(), botConfig.getWithConsole());
	LOG_INFO << botConfig.toString();

	// à construire via un fichier de config ? 
	//Bot::ThresholdService thresholdService({{1.2, 0.75}, {1.5, 0.80}, {1.8, 0.85}, {2, 0.9}});
	Bot::ThresholdService thresholdService({{1.2, 0.8}, {1.5, 0.85}, {1.8, 0.9}, {2, 0.9}});

	// *** KUCOIN ***
	/*Json::Value result24hr;
	Json::Value resultTicker;
	KucoinCPP kucoinCPP("","");
	int i = 2;
	while(--i)
	{
		kucoinCPP.getTicker("ETH-USDT", resultTicker);
		LOG_DEBUG << "result24hr " << resultTicker;
	}*/
	// ***

	// *** GATEIO ***
	ExchangeController::GateioController gateioController{botConfig.getApiKeys()};
	Bot::NewListedCurrencyBot newListedCurrencyBot(
		gateioController
		, botConfig
		, thresholdService);

	newListedCurrencyBot.run();
	//newListedCurrencyBot.watch();
	// ***

	return 0;	
}