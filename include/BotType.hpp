#pragma once
#include <string>

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

struct ThresholdServiceConfig
{
    Threshold lowBound, highBound;
};

struct PriceWatcherConfig
{
    double thresholdPercent;
    unsigned int timeSec;
};

enum class Status
{
    Success,
    Failure
};

struct Price
{
    explicit Price():value(0) {} ;
    explicit Price(double price):value(price) {} ;
    double value;

    operator double() const{ return value;};
};

struct Quantity
{
    explicit Quantity():value(0) {} ;
    explicit Quantity(double quantity):value(quantity) {} ;
    double value;

    operator double() const{ return value;};
};

struct ApiKeys
{
    std::string pub;
    std::string secret;
};

}