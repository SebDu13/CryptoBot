#pragma once

#include <string>
#include "json/json.h"
#include <curl/curl.h>

class KucoinCPP {
	public:
		using SpotTickersResult = Json::Value;

		/* Names match with the api parameters, do not rename */
		enum class Side
		{
			buy,
			sell
		};

		 KucoinCPP(const std::string &api_key,const std::string &secret_key);
		 virtual ~KucoinCPP();
		 void cleanup();

		// Public API
		// pair id looks like "ETH-USDT"
		void send_market_order ( 
		 	const std::string& currency_pair, 
		 	const Side side,
		 	double quoteCurrencyquantity,
		 	Json::Value &json_result ) const;
		void getTicker(const std::string pairId, SpotTickersResult &json_result) const;
        void get24HrStats(const std::string pairId, SpotTickersResult &json_result) const;

		private:
		std::string api_key = "";
		std::string secret_key = "";
		CURL* curl = NULL;

         void getTickersGeneric(const std::string url, SpotTickersResult &json_result) const;
		 void curl_api( std::string &url, std::string &result_json ) const;
		 void curl_api_with_header(const std::string &url
			,const std::vector <std::string> &extra_http_header
			,const std::string &post_data
			,const std::string &action
			,std::string &str_result) const;
		 std::vector <std::string> generateSignedHttpHeader(const std::string& action
		, const std::string& prefix
		, const std::string& body) const;

};