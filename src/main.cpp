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


int main() 
{
	Logger::init(Logger::FilterLevel::Debug);
	std::string api_key	= "api key";
	std::string secret_key 	= "user key";
	BinaCPP::init( api_key , secret_key );
	GateIoCPP::init( api_key , secret_key );

	Json::Value resultCurrencyPairs;
	Json::Value resultLimitOrder;
	//BinaCPP::get_exchangeInfo(result);
	//BinaCPP::get_exchangeInfo(result);
	//GateIoCPP::get_currency_pairs(result);

	do
	{
		auto t_start = std::chrono::high_resolution_clock::now();
		GateIoCPP::get_currency_pairs(resultCurrencyPairs);
		auto t_end = std::chrono::high_resolution_clock::now();
		LOG_INFO << "time to get currency_pairs: " << std::chrono::duration<double, std::milli>(t_end-t_start).count();
		
		t_start = std::chrono::high_resolution_clock::now();
		GateIoCPP::send_limit_order("BTC_USDT"
			,GateIoCPP::Side::buy
			,GateIoCPP::TimeInForce::ioc
			,0.0001
			,60000
			,resultLimitOrder);

		t_end = std::chrono::high_resolution_clock::now();
		LOG_INFO << "time to get send_limit_order: " << std::chrono::duration<double, std::milli>(t_end-t_start).count();
		LOG_INFO << "resultLimitOrder " << resultLimitOrder;
	}while(false);

	//cout << result[1]["id"] << endl;

	/*for(const auto& val : result["symbols"])
	{
 		cout << val["symbol"] << endl;
 		cout << val["quoteAsset"] << endl;
 		break;
	}*/

	return 0;	
}