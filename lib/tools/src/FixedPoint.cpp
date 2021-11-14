#include "FixedPoint.hpp"

namespace tools
{
std::ostream& operator<<(std::ostream& os, const FixedPoint& other)
{
    os << other.toString();
    return os;
}

}