#pragma once
#include <chrono>
#include "BotType.hpp"

namespace Bot
{

class PriceWatcher
{
    public:
    PriceWatcher(PriceWatcherConfig timeSec);

    //return false if the price doesn't move above thresholdPercent (0.1 for 10%)
    // since more than timeSec compare to last call.
    bool isMoving(double price);

    private:
    std::chrono::_V2::system_clock::time_point _startTime;
    double _thresholdPercent;
    unsigned int _timeMilliSec;
    bool _previousPriceIsInit = false;
    double _previousPrice;
};

}