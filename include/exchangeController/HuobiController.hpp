#include "exchangeController/AbstractExchangeController.hpp"
#include "HuobiApi.hpp"

namespace ExchangeController{

class HuobiController: public AbstractExchangeController
{
    public:
    HuobiController(const Bot::ApiKeys& apiKeys);
    virtual ~HuobiController();
    TickerResult getSpotTicker(const std::string& currencyPair) const override;
    std::string getOrderBook(const std::string& currencyPair) const override;
    OrderResult sendOrder(const std::string& currencyPair, const Side side, const Quantity& quantity, const Price& price) const override;
    Quantity computeMaxAmount(const Price& price) const override;
    Quantity getMinOrderSize() const override;
    Quantity getAmountLeft(const OrderResult& buyOrderResult) const override;
    Quantity prepareAccount(const Price& price,const std::optional<Quantity>& maxAmount, const std::optional<Quantity>& quantity) const override;

    private:
    HuobiApi _huobiApi;
};

}