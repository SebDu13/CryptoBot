#include "BotManager.hpp"
#include "logger.hpp"
#include "exchangeController/ExchangeControllerFactory.hpp"
#include "mail.hpp"
#include "chrono.hpp"

namespace Bot{

BotManager::BotManager(const Bot::BotConfig& config)
:_config(config)
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
        if(!config.getQuantity())
            throw std::runtime_error("quantity required with WatchAndSell mode");
    }
    else
    {
        _amount = ExchangeController::ExchangeControllerFactory::create(config)->prepareAccount(config.getLimitBuyPrice()
                                                                                                    , config.getMaxAmount()
                                                                                                    , config.getQuantity());
    }
}

BotManager::~BotManager()
{}

void BotManager::startOnTime()
{
    auto mailConfig = _config.getMailConfig();
	tools::Mail mail({mailConfig.mailServer, mailConfig.login, mailConfig.password});
	
    try
    {
        using namespace std::chrono;
        if(_startTime <= high_resolution_clock::now())
        {
            LOG_ERROR << "Too late.";
            return;
        }
    
        std::vector<std::future<ListingBotStatus>> statusFutures;
        auto bots = prepareBots(statusFutures);
        LOG_INFO << bots.size() << " bots built. Wait for opening..." << _openingTime;

        wait();

        LOG_INFO << "Starting bots...";

        for(std::unique_ptr<ListingBot>& bot: bots)
        {
            CHRONO_THIS_SCOPE;
            if(bot)
            {
                bot->runAsync();
            }

            usleep(_delayBetweenSpawn);
        }

        ListingBotStatus botsStatus;
        // wait that all bots have finished
        for(auto& future : statusFutures)
            if(future.valid())
                botsStatus += future.get();
        
        LOG_INFO << " *** Global status: ***";
        LOG_INFO << botsStatus;

        mail.sendmail(mailConfig.from
            , mailConfig.to
            , std::string(magic_enum::enum_name(_config.getExchange())) + " --> "
                + _config.getPairId() + " "
                + botsStatus.str());
    }
    catch(const std::runtime_error& e)
    {
        /* TODO CHECK WHY EXCEPTIONS THROW FROM A THREAD ARE NOT CAUGHT */ 
        LOG_ERROR << "Exception caught: " << e.what();
        mail.sendmail(mailConfig.from
            , mailConfig.to
            , "Exception caught: " + std::string(e.what()));
    }
    catch(...)
    {
        LOG_ERROR << "Unknown exception caught";
        mail.sendmail(mailConfig.from
            , mailConfig.to
            , "Unknown exception caught. Position may stay OPEN");
    }
}

void BotManager::wait()
{
    while(std::chrono::duration<double, std::micro>(_startTime 
        - std::chrono::high_resolution_clock::now() 
        - std::chrono::milliseconds(_extraDurationMs)).count() > 0);
}

std::vector<std::unique_ptr<ListingBot>> BotManager::prepareBots(std::vector<std::future<ListingBotStatus>>& statusFutures)
{   
    std::vector<std::unique_ptr<ListingBot>> bots;
    for(const auto& limitPrice: _config.computeLimitBuyPrices())
    {
        std::promise<ListingBotStatus> promiseStatus;
        statusFutures.emplace_back(promiseStatus.get_future());
        bots.emplace_back(std::make_unique<ListingBot>(_config
                                                        , _config.getQuantity().value_or(_amount/limitPrice)
                                                        , limitPrice
                                                        , &_stopFlag
                                                        , std::move(promiseStatus)));
    }

    return bots;
}

}