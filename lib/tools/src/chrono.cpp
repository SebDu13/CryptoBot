#include "chrono.hpp"
#include "logger.hpp"
#include <sys/ioctl.h>
#include <termios.h>

namespace tools
{

bool kbhit()
{
    termios term;
    tcgetattr(0, &term);

    termios term2 = term;
    term2.c_lflag &= ~ICANON;
    tcsetattr(0, TCSANOW, &term2);

    int byteswaiting;
    ioctl(0, FIONREAD, &byteswaiting);

    tcsetattr(0, TCSANOW, &term);

    return byteswaiting > 0;
}

Chrono::Chrono(const char* fileName, const size_t line, const char* functionName):
fileName(fileName),
line(line),
functionName(functionName)
{
    startTime = std::chrono::high_resolution_clock::now();
}

Chrono::~Chrono()
{
    stopTime = std::chrono::high_resolution_clock::now();
    BOOST_LOG_SEV(Logger::log, boost::log::trivial::debug) 
        << LOG_LOCATION_PARAM(fileName, line, functionName) 
        << " executed in "
        << std::chrono::duration<double, std::milli>(stopTime-startTime).count() << "ms";
}

}