#pragma once
#include <vector>

namespace Bot
{

struct Threshold
{
    double profit;
    double lossThreshold;

    bool operator<(const Threshold& other) const
    {
        return profit < other.profit;
    }
};
class ThresholdService
{
    public:
    ThresholdService(std::vector<Threshold> thresholds);

    // For 100% put 1
    // 80% 0.8
    // 120% 1.2
    // etc 
    double getLossThreshold(double profitPercent) const;

    private:
    std::vector<Threshold> _thresholds;
};

}