#include "ThresholdService.hpp"
#include <stdexcept>

namespace Bot
{
    ThresholdService::ThresholdService(std::vector<Threshold> thresholds): _thresholds(thresholds)
    {
        if(thresholds.empty())
            throw std::runtime_error("ThresholdService: thresholds vector cannot be null");
        std::sort(_thresholds.begin(), _thresholds.end());
    }

    double ThresholdService::getLossThreshold(double profitPercent) const
    {
        for(auto& threshold :_thresholds)
        {
            if(profitPercent <= threshold.profit)
                return threshold.lossThreshold;
        }

        return _thresholds.back().lossThreshold;
    }
}