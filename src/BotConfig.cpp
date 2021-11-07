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
    ("quantity", po::value<double>(), "Set quantity")
    ("withConsole", "Send logs on console");

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
    else 
    {
        LOG_ERROR << "quantity was not set. --help for more details";
        return Status::Failure;
    }

    if (vm.count("withConsole")) 
        _withConsole = true;
    else
        LOG_INFO << "withConsole was set to " << false;

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
    stream << "quantity=" << _quantity.value << std::endl;
    stream << "withConsole=" << _withConsole << std::endl;

    return stream.str();
}
}