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
    ("quantity", po::value<double>(), "Set quantity");

    po::variables_map vm;        
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm); 

    if (vm.count("help")) 
    {
        LOG_INFO << desc;
        return Status::Failure;
    }

    if (vm.count("id")) 
    {
        _pairId = vm["id"].as<std::string>();
        LOG_INFO << "id was set to " << _pairId;
    }
    else 
    {
        LOG_ERROR << "id was not set. --help for more details";
        return Status::Failure;
    }

    if (vm.count("limitPrice")) 
    {
        _limitBuyPrice = Price(vm["limitPrice"].as<double>());
        LOG_INFO << "limitPrice was set to " << _limitBuyPrice;
    }
    else 
    {
        LOG_ERROR << "limitPrice was not set. --help for more details";
        return Status::Failure;
    }

    if (vm.count("quantity")) 
    {
        _quantity = Quantity(vm["quantity"].as<double>());
        LOG_INFO << "quantity was set to " << _quantity;
    }
    else 
    {
        LOG_ERROR << "quantity was not set. --help for more details";
        return Status::Failure;
    }

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
}