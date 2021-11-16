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
#include "exchangeController/ExchangeControllerFactory.hpp"
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

	// *** KUCOIN ***
	/*Json::Value result24hr;
	Json::Value resultTicker;
	Json::Value resultOrder;
	auto apiKeys = botConfig.getApiKeys(Bot::Exchange::Kucoin);*/

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

	/*ExchangeController::KucoinController kucoinController(apiKeys);
	Bot::ListingBot newListedCurrencyBot(
		kucoinController
		, botConfig);
	//newListedCurrencyBot.run();
	newListedCurrencyBot.watch();
	/*LOG_DEBUG << kucoinController.sendOrder(botConfig.getPairId()
		, ExchangeController::Side::buy
		, *botConfig.getQuantity()
		, botConfig.getLimitBuyPrice()).toString();*/

	// ***

	/*Bot::ListingBot listingBot(
		ExchangeController::ExchangeControllerFactory::create(botConfig)
		, botConfig);

	listingBot.run();*/
	//listingBot.watch();

    /*std::tm t = {};
    std::istringstream ss("13:00");

    if (ss >> std::get_time(&t, "%H:%M"))
    {
        std::cout << std::put_time(&t, "%c") << "\n"
                  << std::mktime(&t) << "\n";
    }
    else
    {
        std::cout << "Parse failed\n";
    }*/



	std::time_t time = std::time(0);   // get today date
    std::tm* now = std::gmtime(&time); // convert to utc struct time
	std::tm later = *now;
	later.tm_sec =0;

	LOG_DEBUG << later.tm_zone;
	std::istringstream ss("16:10");
	ss >> std::get_time(&later, "%H:%M"); // add hour 
	LOG_DEBUG << later.tm_zone;

	std::cout << (later.tm_year) << '-' 
	<< (later.tm_mon) << '-'
	<<  later.tm_mday << '-'
	<<  later.tm_hour << ':'
	<<  later.tm_min
	<< "\n";


	LOG_DEBUG << timegm(&later);
	auto time_t_later = std::chrono::system_clock::from_time_t(timegm(&later));
	auto time_t_later_diff = time_t_later - std::chrono::high_resolution_clock::now();
	//LOG_DEBUG << time_t_later_diff.count();
	LOG_DEBUG << std::chrono::duration<double, std::micro>(time_t_later_diff).count() << "ms";


	//LOG_DEBUG << std::chrono::high_resolution_clock::now();

	//auto laterCast = std::chrono::duration_cast<std::chrono::microseconds>(timegm(&later));
	//later - nowFromClock;

	long long count;
	//if(time_t_later > std::chrono::high_resolution_clock::now())
	//{
		do
		{
			count = std::chrono::duration<double, std::micro>(time_t_later - std::chrono::high_resolution_clock::now() /*-std::chrono::milliseconds(35)*/).count();
			LOG_DEBUG << count;
		}while(  count > 0);
	//}

	//Bot::ListingBot listingBot1(botConfig);

	//Bot::ListingBot listingBot2(botConfig);

	/*std::thread thread1([&listingBot1](){
		listingBot1.justBuy();
	});

	std::thread thread2([&listingBot2](){
		listingBot2.justBuy();
	});

	if(thread1.joinable())
		thread1.join();
	if(thread2.joinable())
		thread2.join();*/
	Bot::BotManager botmanager(botConfig);
	botmanager.startOnTime();

	return 0;	
}