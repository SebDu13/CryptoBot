#pragma once

#include <boost/log/trivial.hpp>

#define __FILENAME__ std::max<const char*>(__FILE__,\
    std::max(strrchr(__FILE__, '\\')+1, strrchr(__FILE__, '/')+1))
#define LOG_LOCATION std::string("") + __FILENAME__ + ":" + std::to_string(__LINE__) + " " + __FUNCTION__ + " "

#define LOG_DEBUG   BOOST_LOG_SEV(Logger::log, boost::log::trivial::debug) << LOG_LOCATION
#define LOG_INFO    BOOST_LOG_SEV(Logger::log, boost::log::trivial::info) << LOG_LOCATION
#define LOG_WARNING BOOST_LOG_SEV(Logger::log, boost::log::trivial::warning) << LOG_LOCATION
#define LOG_ERROR   BOOST_LOG_SEV(Logger::log, boost::log::trivial::error) << LOG_LOCATION

struct Logger
{
	enum class FilterLevel
	{
		Verbose,
		Debug
	};

	inline static boost::log::sources::severity_logger<boost::log::trivial::severity_level> log;
	static void init(FilterLevel filterLevel);	
};