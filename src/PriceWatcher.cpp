#include <cmath>
#include "PriceWatcher.hpp"
#include "logger.hpp"

namespace Bot
{

PriceWatcher::PriceWatcher(PriceWatcherConfig config)
: _thresholdPercent(config.thresholdPercent)
, _timeMilliSec(config.timeSec *1000)
{
    _startTime = std::chrono::high_resolution_clock::now();
}

bool PriceWatcher::isMoving(double price)
{
    if(!_previousPriceIsInit)
    {
        _previousPrice = price;
        _previousPriceIsInit = true;
        return true;
    }
    
    const double movePercent = abs(price - _previousPrice)/_previousPrice;
    if(movePercent > _thresholdPercent)
    {
        _startTime = std::chrono::high_resolution_clock::now();
        LOG_DEBUG << "movePercent=" << movePercent << " reset timer";
    }

    if(auto durationMs = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now()-_startTime).count();
         durationMs < _timeMilliSec)
        return true;

    return false;
}

}