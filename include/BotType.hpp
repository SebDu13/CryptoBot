#pragma once

namespace Bot
{

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