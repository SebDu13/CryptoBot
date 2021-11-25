#include "BotManager.hpp"
#include "logger.hpp"
#include "exchangeController/ExchangeControllerFactory.hpp"

namespace Bot{

BotManager::BotManager(const Bot::BotConfig& config)
:_config(config)
, _botNumber(config.getThreadNumber())
, _extraDurationMs(config.getDurationBeforeStartMs())
, _delayBetweenSpawn(config.getDelayBetweenBotsSpawnUs())
, _openingTime(config.getStartTime())
{
    std::time_t time = std::time(0);   // get today date
    std::tm* now = std::gmtime(&time); // convert to utc struct time
	std::tm later = *now;
	later.tm_sec =0;
    std::istringstream ss(_openingTime);
	ss >> std::get_time(&later, "%H:%M"); // add hour
    auto t_time_later = timegm(&later);
    _startTime = std::chrono::system_clock::from_time_t(t_time_later);

    if(config.getMode() == RunningMode::WatchAndSell)
    {
        if(auto quantityOpt = config.getQuantity())
            _quantity = *quantityOpt;
        else
            throw std::runtime_error("quantity required with WatchAndSell mode");
    }
    else
    {
        _quantity = ExchangeController::ExchangeControllerFactory::create(config)->prepareAccount(config.getLimitBuyPrice()
                                                                                                    , config.getMaxAmount()
                                                                                                    , config.getQuantity());
    }

    LOG_INFO << "Quantity computed: " << _quantity;
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
        _listingBots.emplace_back(std::make_unique<ListingBot>(_config, _quantity));

    LOG_INFO << _botNumber << " bots built. Wait for opening..." << _openingTime;

    wait();

    LOG_INFO << "Starting bots...";

    for(std::unique_ptr<ListingBot>& bot: _listingBots)
    {
        if(bot)
            bot->runAsync(&_stopFlag);
        usleep(_delayBetweenSpawn);
    }
}

void BotManager::wait()
{
    while(std::chrono::duration<double, std::micro>(_startTime 
        - std::chrono::high_resolution_clock::now() 
        - std::chrono::milliseconds(_extraDurationMs)).count() > 0);
}

}