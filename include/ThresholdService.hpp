#pragma once
#include <vector>
#include "BotType.hpp"

namespace Bot
{

class ThresholdService
{
    public:
    ThresholdService(ThresholdServiceConfig config);

    // For 100% put 1
    // 80% 0.8
    // 120% 1.2
    // Just return the lineary extrapolated threshold between two bounds
    double getLossThreshold(double profitPercent) const;

    private:
    ThresholdServiceConfig _config;
    double _a, _b; // from linear equation ax+b
};

}