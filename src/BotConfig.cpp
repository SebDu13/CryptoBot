#include "BotConfig.hpp"
#include <boost/program_options.hpp>
#include "logger.hpp"

namespace Bot
{

Status BotConfig::loadOptionsFromMain(int argc, char **argv)
{
    namespace po = boost::program_options;

    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "Make sure to have at least limitPrice*quantity available on the wallet")
    ("id", po::value<std::string>(), "Set new currency identifier. Ex: ETH_USDT")
    ("limitPrice", po::value<double>(), "Set limit price. Put a large price to have more chance to be executed")
    ("quantity", po::value<double>(), "Set quantity. If not set, it will use the total amount available on the wallet")
    ("withConsole", "Send logs on console")
    ("greedy", "Duration used to sell if the the price doesn't move and price thresholds are more permissive.");

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm); 

    if (vm.count("help")) 
    {
        LOG_INFO << desc;
        return Status::Failure;
    }

    if (vm.count("id")) 
        _pairId = vm["id"].as<std::string>();
    else 
    {
        LOG_ERROR << "id was not set. --help for more details";
        return Status::Failure;
    }

    if (vm.count("limitPrice")) 
        _limitBuyPrice = Price(vm["limitPrice"].as<double>());
    else 
    {
        LOG_ERROR << "limitPrice was not set. --help for more details";
        return Status::Failure;
    }

    if (vm.count("quantity")) 
        _quantity = Quantity(vm["quantity"].as<double>());

    if (vm.count("withConsole")) 
        _withConsole = true;
    else
        LOG_INFO << "withConsole was set to " << false;
    
    if (vm.count("greedy")) 
        _greedyMode = true;

    return Status::Success;
}

ApiKeys BotConfig::getApiKeys() const
{
    ApiKeys apiKeys;
    const char* apiKeyEnv = "GATEIO_K";
    const char* secretKeyEnv = "GATEIO_S";

    if(auto key = std::getenv(apiKeyEnv))
        apiKeys.pub = std::string(key);
    else
        LOG_WARNING << apiKeyEnv << " not set or null";

    if(auto key = std::getenv(secretKeyEnv))
        apiKeys.secret = std::string(key);
    else
        LOG_WARNING << secretKeyEnv << " not set or null";

    return apiKeys;
}

std::string BotConfig::toString() const
{
    std::stringstream stream;
    stream << "*** BotConfig: ***" << std::endl;
    stream << "pairId=" << _pairId << std::endl;
    stream << "limitBuyPrice=" << _limitBuyPrice.value << std::endl;
    if(_quantity) stream << "quantity=" << *_quantity << std::endl;
    stream << "withConsole=" << _withConsole << std::endl;
    stream << "greedy=" << _greedyMode << std::endl;

    return stream.str();
}

TimeThresholdConfig BotConfig::getTimeThresholdConfig() const
{
    TimeThreshold lowBound, highBound;
    double thresholdPercent;
    if(_greedyMode)
    {
        lowBound.profit = 1.2;
        lowBound.timeSec=8;
        highBound.profit = 2;
        highBound.timeSec=3;
        thresholdPercent=0.15;
    }
    else
    {
        lowBound.profit = 1.2;
        lowBound.timeSec=5;
        highBound.profit = 1.8;
        highBound.timeSec=3;
        thresholdPercent=0.1;
    }
    return {.priceThresholdPercent=thresholdPercent, .lowBound = lowBound, .highBound = highBound};
}

PriceThresholdConfig BotConfig::getPriceThresholdConfig() const
{   
    PriceThreshold lowBound, highBound;
    if(_greedyMode)
    {
        lowBound.profit = 1.2;
        lowBound.lossThreshold=0.75;
        highBound.profit = 2;
        highBound.lossThreshold=0.9;
    }
    else
    {
        lowBound.profit = 1.2;
        lowBound.lossThreshold=0.8;
        highBound.profit = 1.8;
        highBound.lossThreshold=0.9;
    }
    return {.lowBound = lowBound, .highBound = highBound};
}

}