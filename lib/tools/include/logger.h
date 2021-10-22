#pragma once

#include <boost/log/trivial.hpp>

#define LOG_DEBUG   BOOST_LOG_SEV(Logger::log, boost::log::trivial::debug)
#define LOG_INFO    BOOST_LOG_SEV(Logger::log, boost::log::trivial::info)
#define LOG_WARNING BOOST_LOG_SEV(Logger::log, boost::log::trivial::warning)
#define LOG_ERROR   BOOST_LOG_SEV(Logger::log, boost::log::trivial::error)

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