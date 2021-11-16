#include "BotManager.hpp"
#include "logger.hpp"

namespace Bot{

BotManager::BotManager(const Bot::BotConfig& config):_config(config)
{
    std::time_t time = std::time(0);   // get today date
    std::tm* now = std::gmtime(&time); // convert to utc struct time
	std::tm later = *now;
	later.tm_sec =0;
    std::istringstream ss("21:33");
	ss >> std::get_time(&later, "%H:%M"); // add hour
    _startTime = std::chrono::system_clock::from_time_t(timegm(&later));
}

BotManager::~BotManager()
{}

void BotManager::startOnTime()
{
    using namespace std::chrono;
    if(_startTime <= high_resolution_clock::now())
    {
        LOG_ERROR << "Too late.";
        return;
    }
 
    for(auto i = 0; i< _botNumber; ++i)
        _listingBots.emplace_back(std::make_unique<ListingBot>(_config));

    LOG_INFO << _botNumber << " bots built. Wait for opening...";

    long long count;
    do
    {
        count = std::chrono::duration<double, std::micro>(_startTime - std::chrono::high_resolution_clock::now() -std::chrono::milliseconds(35)).count();
    }while(  count > 0);

    //while(duration<long, std::micro>(_startTime - high_resolution_clock::now()).count() > 0);

    LOG_INFO << "Starting bots...";

    for(std::unique_ptr<ListingBot>& bot: _listingBots)
    {
        if(bot)
            bot->runAsync(&_stopFlag);
        usleep(500); // 500 micro seconds delay between each bot
    }
}

}