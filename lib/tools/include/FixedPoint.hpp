#pragma once
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <iostream>
#include <iomanip>

namespace tools{
struct FixedPoint
{
    using ValueType = boost::multiprecision::cpp_dec_float_50;
    //using ValueType = boost::multiprecision::number<cpp_dec_float<50> >;


    explicit FixedPoint():value(0) {} ;
    explicit FixedPoint(std::string price):value(price) {} ;
    FixedPoint(ValueType value):value(value) {} ;

    friend std::ostream& operator<<(std::ostream& os, const FixedPoint& other);
    explicit operator double() const{ return value.convert_to<double>();};
    FixedPoint operator*(const FixedPoint& other) const{ return FixedPoint(value * other.value); };
    FixedPoint operator+(const FixedPoint& other) const{ return FixedPoint(value + other.value); };
    FixedPoint operator-(const FixedPoint& other) const{ return FixedPoint(value - other.value); };
    FixedPoint operator/(const FixedPoint& other) const{ return FixedPoint(value / other.value); };
    bool operator==(const FixedPoint& other) const{ return value == other.value; };
    bool operator<(const FixedPoint& other) const{ return value < other.value; };
    bool operator>(const FixedPoint& other) const{ return value > other.value; };
    bool operator>=(const FixedPoint& other) const{ return value >= other.value; };
    bool operator<=(const FixedPoint& other) const{ return value <= other.value; };
    
    std::string toString() const
    { 
        std::ostringstream os;
        os << std::fixed << value;
        return os.str();
    };
    std::string toStringExact() const
    {
        std::ostringstream os;
        os << std::fixed << std::setprecision(std::numeric_limits<double>::digits10) << value;
        return os.str();
    };

    ValueType value;
};

std::ostream& operator<<(std::ostream& os, const FixedPoint& other);

}

using Price = tools::FixedPoint;
using Quantity = tools::FixedPoint;