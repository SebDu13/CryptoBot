#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <json/json.h>
#include <unistd.h>
#include <chrono>

#include "logger.hpp"
#include "binacpp.h"
#include "gateiocpp.h"
#include "exchangeController/GateioController.hpp"

namespace {
 void setGateIoApiKey(std::string& apiKey, std::string& secretKey)
 {
	const char* apiKeyEnv = "GATEIO_K";
	const char* secretKeyEnv = "GATEIO_S";

	if(auto key = std::getenv(apiKeyEnv))
		apiKey = std::string(key);
	else
		LOG_WARNING << apiKeyEnv << " not set or null";


	if(auto key = std::getenv(secretKeyEnv))
		secretKey = std::string(key);
	else
		LOG_WARNING << secretKeyEnv << " not set or null";
 }
}


int main() 
{
	Logger::init(Logger::FilterLevel::Debug);
	std::string apiKey, secretKey;
	setGateIoApiKey(apiKey, secretKey);

	std::string resultCurrencyPairs;
	Json::Value resultLimitOrder;
	Json::Value result;
	//BinaCPP::init( apiKey , secretKey );
	//GateIoCPP gateIoAPI( apiKey , secretKey );
	//gateIoAPI.get_currency_pairs(resultCurrencyPairs);

	ExchangeController::GateioController gateioController(apiKey, secretKey);
	//auto newpair = gateioController.getNewCurrencyPairSync();
	auto spotTicker = gateioController.getSpotTicker("ETH_USDT");
	auto resultSendOrder = gateioController.sendOrder("ETH_USDT", ExchangeController::Side::buy, 1, "10");
	/*LOG_DEBUG << "id " << newpair.id;
    LOG_DEBUG << "base " << newpair.base;
    LOG_DEBUG << "quote " << newpair.quote;*/
	
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