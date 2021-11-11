#include "ThresholdService.hpp"
#include <stdexcept>

namespace Bot
{
    ThresholdService::ThresholdService(ThresholdServiceConfig config): _config(config)
    {
        _a = (_config.highBound.lossThreshold - _config.lowBound.lossThreshold) 
            / (_config.highBound.profit - _config.lowBound.profit);

        _b = _config.highBound.lossThreshold - _a * _config.highBound.profit;
    }

    double ThresholdService::getLossThreshold(double profitPercent) const
    {
        if(profitPercent <= _config.lowBound.profit)
            return _config.lowBound.lossThreshold;
        if(profitPercent >= _config.highBound.profit)
            return _config.highBound.lossThreshold;

        return _a * profitPercent + _b;
    }
}