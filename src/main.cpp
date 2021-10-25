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

	//BinaCPP::init( apiKey , secretKey );
	GateIoCPP::init( apiKey , secretKey );

	Json::Value resultCurrencyPairs;
	Json::Value resultLimitOrder;
	Json::Value result;
	//BinaCPP::get_exchangeInfo(result);
	//BinaCPP::send_order("","","","sebseb",1,2,"",3,3,3,result);
	//BinaCPP::get_exchangeInfo(result);
	//GateIoCPP::get_currency_pairs(result);

	int i=1;
	do
	{
		GateIoCPP::get_currency_pairs(resultCurrencyPairs);
		GateIoCPP::send_limit_order("BTC_USDT"
			,GateIoCPP::Side::buy
			,GateIoCPP::TimeInForce::ioc
			,0.0001
			,60000
			,resultLimitOrder);

		LOG_INFO << "resultLimitOrder " << resultLimitOrder;
		i--;
	}while(i>0);

	//cout << result[1]["id"] << endl;

	/*for(const auto& val : result["symbols"])
	{
 		cout << val["symbol"] << endl;
 		cout << val["quoteAsset"] << endl;
 		break;
	}*/

	return 0;	
}