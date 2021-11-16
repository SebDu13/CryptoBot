#pragma once
#include <chrono>
#include <vector>
#include <memory>
#include "ListingBot.hpp"
#include <atomic>

namespace Bot{

class BotManager
{
    public:
    BotManager(const Bot::BotConfig& config);
    virtual ~BotManager();
    void startOnTime();

    private:
    const Bot::BotConfig& _config;
    const int _botNumber = 10;
    std::vector<std::unique_ptr<ListingBot>> _listingBots;
    const int _extraDurationUs = 35000;
    std::chrono::system_clock::time_point _startTime;
    std::atomic<bool> _stopFlag = false;
};

}