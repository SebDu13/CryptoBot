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


int main(int argc, char **argv) 
{
	using namespace Bot;

	BotConfig botConfig;
	if(botConfig.loadOptionsFromMain(argc, argv) == Status::Failure)
		return -1;

	Logger::init(Logger::FilterLevel::Debug, botConfig.getPairId(), botConfig.getWithConsole());
	LOG_INFO << botConfig.toString();

	std::string resultCurrencyPairs;
	Json::Value resultLimitOrder;
	Json::Value result;
	//BinaCPP::init( apiKey , secretKey );
	//GateIoCPP gateIoAPI( apiKey , secretKey );
	//gateIoAPI.get_currency_pairs(resultCurrencyPairs);

	auto apiKey = botConfig.getApiKeys();
	ExchangeController::GateioController gateioController(apiKey.pub, apiKey.secret);
	//auto newpair = gateioController.getNewCurrencyPairSync("USDT");
	//LOG_DEBUG << "id " << newpair.id;
    //LOG_DEBUG << "base " << newpair.base;
    //LOG_DEBUG << "quote " << newpair.quote;
	/*while(true)
	{
		auto spotTicker = gateioController.getSpotTicker("ETH_USDT");
		LOG_DEBUG << spotTicker.toString();
	}*/
	/*double quantity = 5;
	auto resultBuyOrder = gateioController.sendOrder("XRP_USDT", ExchangeController::Side::buy, quantity, 10);
	LOG_DEBUG << "resultBuyOrder " << resultBuyOrder.toString();

	std::this_thread::sleep_for(std::chrono::milliseconds(3000));

 	auto resultSellOrder = gateioController.sendOrder("XRP_USDT", ExchangeController::Side::sell
	 , resultBuyOrder.amount - resultBuyOrder.fee
	 , (resultBuyOrder.fillPrice / resultBuyOrder.amount)*0.2);
	LOG_DEBUG << "resultSellOrder " << resultSellOrder.toString();*/

	Bot::NewListedCurrencyBot newListedCurrencyBot(gateioController
		, botConfig.getPairId()
		, botConfig.getLimitBuyPrice()
		, botConfig.getQuantity());

	newListedCurrencyBot.run();

	//BinaCPP::get_exchangeInfo(result);
	//BinaCPP::send_order("","","","sebseb",1,2,"",3,3,3,result);
	//BinaCPP::get_exchangeInfo(result);
	//gateIoAPI.get_currency_pairs(result);

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