#pragma once
#include <string>
#include <vector>
#include <optional>
#include "BotType.hpp"

namespace Bot
{
    class BotConfig
    {
        public:
        Status loadOptionsFromMain(int argc, char **argv);

        std::string getPairId() const {return _pairId;};
        Price getLimitBuyPrice() const {return _limitBuyPrice;};
        std::optional<Quantity> getQuantity() const {return _quantity;};
        ApiKeys getApiKeys() const;
        bool getWithConsole() const {return _withConsole;};
        std::string toString() const;
        PriceWatcherConfig getPriceWatcherConfig() const;
        ThresholdServiceConfig getThresholConfig() const;

        private:
        std::string _pairId;
        Price _limitBuyPrice; // generally large
        std::optional<Quantity> _quantity;
        bool _withConsole;
        bool _greedyMode; // good for not listed token yet
    };
}