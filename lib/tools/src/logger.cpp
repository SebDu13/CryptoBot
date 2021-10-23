#include "logger.hpp"
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/log/attributes/attribute.hpp>

void Logger::init(FilterLevel filterLevel) 
{
    boost::log::add_common_attributes();

    if (filterLevel == FilterLevel::Debug)
    boost::log::core::get()->set_filter(boost::log::trivial::debug <= boost::log::trivial::severity);
    else if (filterLevel == FilterLevel::Verbose)
    boost::log::core::get()->set_filter(boost::log::trivial::info <= boost::log::trivial::severity);
    else
    boost::log::core::get()->set_filter(boost::log::trivial::warning <= boost::log::trivial::severity);

    // log format: [TimeStamp] [Severity Level] Log message
    auto fmtTimeStamp = boost::log::expressions::
    format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S:%f");
    auto fmtSeverity = boost::log::expressions::
    attr<boost::log::trivial::severity_level>("Severity");
    auto fmtThreadID = boost::log::expressions::
    attr<boost::log::attributes::current_thread_id::value_type>("ThreadID");

    boost::log::formatter logFmt =
    boost::log::expressions::format("[%1%] [%2%] [%3%] %4%")
    % fmtThreadID
    % fmtTimeStamp
    % fmtSeverity
    % boost::log::expressions::smessage;

    auto console_sink = boost::log::add_console_log(std::cout);
    console_sink->set_formatter(logFmt);
}
