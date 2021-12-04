#pragma once
#include <chrono>
#include <vector>
#include <memory>
#include "ListingBot.hpp"
#include <atomic>
#include <string>
#include <future>

namespace Bot{

class BotManager
{
    public:
    BotManager(const Bot::BotConfig& config);
    virtual ~BotManager();
    void startOnTime();

    private:
    const Bot::BotConfig& _config;
    Quantity _amount;
    const unsigned int _extraDurationMs;
    const unsigned int _delayBetweenSpawn;
    const std::string _openingTime;
    std::chrono::system_clock::time_point _startTime;
    std::atomic<bool> _stopFlag = false;

    std::vector<std::unique_ptr<ListingBot>> prepareBots(std::vector<std::future<ListingBotStatus>>& statusFutures);
    void wait();
};

}