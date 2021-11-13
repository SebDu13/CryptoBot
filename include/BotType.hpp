#pragma once
#include <string>
#include "LinearExtrapoler.hpp"

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