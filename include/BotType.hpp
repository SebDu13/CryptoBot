#pragma once
#include <string>
#include "LinearExtrapoler.hpp"
#include "FixedPoint.hpp"

namespace Bot
{

// The idea here is more the profit increasing,
// more the lossThreshold becoming small and less risky
struct PriceThreshold: public tools::Extrapolable
{
    double profit;
    double lossThreshold;

    double getX() const override { return profit;};
    double getY() const override { return lossThreshold;};
};

// The idea here is more the profit increasing,
// more the timeSec in position if price doesn't move becoming small and less risky
struct TimeThreshold: tools::Extrapolable
{
    double profit;
    unsigned int timeSec;

    double getX() const override { return profit;};
    double getY() const override { return timeSec;};
};

struct PriceThresholdConfig
{
    PriceThreshold lowBound, highBound;
};

struct TimeThresholdConfig
{
    double priceThresholdPercent;
    TimeThreshold lowBound, highBound;
};

enum class Exchange
{
    Gateio,
    Kucoin
};

enum class Status
{
    Success,
    Failure
};

struct ApiKeys
{
    std::string pub;
    std::string secret;
    std::string passphrase;
};

enum class RunningMode
{
    Normal,
    WatchAndSell
};

}