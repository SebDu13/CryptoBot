#pragma once

#include <string>
#include "json/json.h"
#include <curl/curl.h>
#include "FixedPoint.hpp"
#include "curloptions.hpp"

class HuobiApi {
	public:
	  	static constexpr char kUrlBase[] = "https://api.huobi.pro";
  		static constexpr char kUserAgent[] = "Huobi C++ API Client";

		using SpotTickersResult = Json::Value;

		/* Names match with the api parameters, do not rename */
		enum class Side
		{
			buy,
			sell
		};

		/* Names match with the api parameters, do not rename */
		// Good Till Canceled GTC, Good Till Time GTT, Immediate Or Cancel IOC, and Fill Or Kill FOK.
		enum class TimeInForce	
		{
			GTC,
			GTT,
			IOC,
			FOK
		};

		 HuobiApi(const std::string &api_key,const std::string &secret_key, const std::string &pass_phrase);
		 virtual ~HuobiApi();
		 void cleanup();

		// Public API
		// pair id looks like "ETH-USDT"
		void sendLimitOrder ( 
			const std::string& currency_pair, 
			const Side side,
			const TimeInForce timeInForce,
			const Quantity& quantity,
			const Price& price,
			Json::Value &json_result ) const;
		
		void getOrder( const std::string& orderId, Json::Value &json_result ) const;

		void getTicker(const std::string& pairId, SpotTickersResult &json_result) const;
		void getAccounts(Json::Value &json_result) const;

		void getMexcTicker(const std::string& pairId, SpotTickersResult &json_result) const;
		void getMexcTickersGeneric(const std::string& url, SpotTickersResult &json_result) const;

		private:
		std::string api_key = "";
		std::string secret_key = "";
		std::string passPhrase = "";
		CURL* curl = NULL;

         void getTickersGeneric(const std::string& url, SpotTickersResult &json_result) const;
		 void curl_api( std::string &url, std::string &result_json ) const;
		 void curl_api_with_header(const std::string &url
			,const std::vector <std::string> &extra_http_header
			,const std::string &post_data
			,const std::string &action
			,std::string &str_result) const;
		 std::vector <std::string> generateSignedHttpHeader(const std::string& action
		, const std::string& prefix
		, const std::string& body) const;
		Json::Value PrivateQuery(cct::CurlOptions::RequestType requestType, std::string_view method, const cct::CurlPostData& postdata = cct::CurlPostData()) const;

};