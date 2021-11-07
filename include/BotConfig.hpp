#pragma once
#include <string>
#include "BotType.hpp"

namespace Bot
{
    class BotConfig
    {
        public:
        Status loadOptionsFromMain(int argc, char **argv);

        std::string getPairId() const {return _pairId;};
        Price getLimitBuyPrice() const {return _limitBuyPrice;};
        Quantity getQuantity() const {return _quantity;};
        ApiKeys getApiKeys() const;
        bool getWithConsole() const {return _withConsole;};
        std::string toString() const;

        private:
        std::string _pairId;
        Price _limitBuyPrice; // generally large
        Quantity _quantity;
        bool _withConsole;
    };
}