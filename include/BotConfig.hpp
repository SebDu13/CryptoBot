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
        ApiKeys getApiKeys(Exchange exchange) const;
        bool getWithConsole() const {return _withConsole;};
        std::string toString() const;
        TimeThresholdConfig getTimeThresholdConfig() const;
        PriceThresholdConfig getPriceThresholdConfig() const;

        private:
        std::string _pairId = "";
        Price _limitBuyPrice = Price(); // generally large
        std::optional<Quantity> _quantity = std::nullopt;
        bool _withConsole = false;
        bool _greedyMode = false; // good for not listed token yet
    };
}