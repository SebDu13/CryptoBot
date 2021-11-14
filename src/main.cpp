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
#include "exchangeController/KucoinController.hpp"
#include "magic_enum.hpp"
#include "ListingBot.hpp"
#include "BotType.hpp"
#include "BotConfig.hpp"
#include "tools.hpp"

#include <boost/multiprecision/cpp_dec_float.hpp>


int main(int argc, char **argv) 
{
	using namespace Bot;

	BotConfig botConfig;
	if(botConfig.loadOptionsFromMain(argc, argv) == Status::Failure)
		return -1;

	Logger::init(Logger::FilterLevel::Debug, botConfig.getPairId(), botConfig.getWithConsole());
	LOG_INFO << botConfig.toString();

	// *** KUCOIN ***
	Json::Value result24hr;
	Json::Value resultTicker;
	Json::Value resultOrder;
	auto apiKeys = botConfig.getApiKeys(Bot::Exchange::Kucoin);

	/*using boost::multiprecision::cpp_dec_float_50;
	cpp_dec_float_50 un = cpp_dec_float_50("0.000000000001");
	cpp_dec_float_50 deux = cpp_dec_float_50("0.00001");
	LOG_DEBUG << std::fixed << std::setprecision(std::numeric_limits<double>::max_digits10) << (deux - un).convert_to<std::string>();*/

	//KucoinCPP kucoinCPP(apiKeys.pub, apiKeys.secret, apiKeys.passphrase);
	/*int i = 10;
	while(true)
	{
		kucoinCPP.getTicker(botConfig.getPairId(), resultTicker);
		LOG_DEBUG << "getTicker " << resultTicker;
		kucoinCPP.get24HrStats(botConfig.getPairId(), resultTicker);
		LOG_DEBUG << "result24hr " << resultTicker;
	}*/
	/*kucoinCPP.sendLimitOrder(botConfig.getPairId()
		, KucoinCPP::Side::buy
		, KucoinCPP::TimeInForce::IOC
		, *botConfig.getQuantity()
		, botConfig.getLimitBuyPrice()
		,resultOrder);*/
	/*kucoinCPP.getOrder("61911e97fe8b030001e1333d", resultOrder );
	LOG_DEBUG << "resultOrder " << resultOrder;*/

	ExchangeController::KucoinController kucoinController(apiKeys);
	Bot::ListingBot newListedCurrencyBot(
		kucoinController
		, botConfig);
	newListedCurrencyBot.run();
	/*LOG_DEBUG << kucoinController.sendOrder(botConfig.getPairId()
		, ExchangeController::Side::buy
		, *botConfig.getQuantity()
		, botConfig.getLimitBuyPrice()).toString();*/

	// ***

	// *** GATEIO ***
	/*ExchangeController::GateioController gateioController{botConfig.getApiKeys(Bot::Exchange::Gateio)};
	Bot::ListingBot newListedCurrencyBot(
		gateioController
		, botConfig);

	newListedCurrencyBot.run();*/
	//newListedCurrencyBot.watch();

	return 0;	
}