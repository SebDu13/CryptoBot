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
#include "gateiocpp.h"
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

	auto apiKeys = botConfig.getApiKeys();
	ExchangeController::GateioController gateioController(apiKeys.pub, apiKeys.secret);

	// à construire via un fichier de config ? 
	Bot::ThresholdService thresholdService({{1.2, 0.7}, {1.5, 0.75}, {1.8, 0.8}, {2, 0.9}});

	Bot::NewListedCurrencyBot newListedCurrencyBot(gateioController
		, botConfig.getPairId()
		, botConfig.getLimitBuyPrice()
		, botConfig.getQuantity()
		, thresholdService);

	//newListedCurrencyBot.run();
	//newListedCurrencyBot.watch();


	int i=1;
	/*do
	{
		gateIoAPI.get_currency_pairs(resultCurrencyPairs);
		//LOG_INFO << "resultCurrencyPairs " << resultCurrencyPairs;

		gateIoAPI.send_limit_order("BTC_USDT"
			,gateIoAPI.Side::buy
			,gateIoAPI.TimeInForce::ioc
			,0.0001
			,60000
			,resultLimitOrder);
		LOG_INFO << "resultLimitOrder " << resultLimitOrder;

		gateIoAPI.get_spot_tickers("BTC_USDT", result);
		LOG_INFO << "result get_spot_tickers " << result;
		i--;
	}while(i>0);*/

	//cout << result[1]["id"] << endl;

	/*for(const auto& val: resultCurrencyPairs)
	{
		LOG_DEBUG << val["id"];
	}*/

	/*for(const auto& val : result["symbols"])
	{
 		cout << val["symbol"] << endl;
 		cout << val["quoteAsset"] << endl;
 		break;
	}*/
	return 0;	
}