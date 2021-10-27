#pragma once

#include <string>
#include "json/json.h"
#include <curl/curl.h>

class GateIoCPP {
	public:
		using CurrencyPairsResult = Json::Value;
		using SpotTickersResult = Json::Value;

		/* Names match with the api parameters, do not rename */
		enum class Side
		{
			buy,
			sell
		};

		/*
		- gtc: GoodTillCancelled
		- ioc: ImmediateOrCancelled, taker only
		- poc: PendingOrCancelled, makes a post-only order 
				that always enjoys a maker fee*/
		enum class TimeInForce
		{
			gtc,
			ioc,
			poc
		};

		 GateIoCPP( std::string &api_key, std::string &secret_key);
		 virtual ~GateIoCPP();
		 void cleanup();

		// Public API
		 void get_currency_pairs( CurrencyPairsResult &json_result);
		 void send_limit_order( 
			const std::string& currency_pair,
			const Side side,
			const TimeInForce timeInForce,
			double quantity,
			double price,
			Json::Value &json_result );
		 void get_spot_tickers(const std::string& currencyPair, SpotTickersResult &json_result);

		private:
		std::string api_key = "";
		std::string secret_key = "";
		CURL* curl = NULL;

		 void curl_api( std::string &url, std::string &result_json );
		 void curl_api_with_header(const std::string &url
			,const std::vector <std::string> &extra_http_header
			,const std::string &post_data
			,const std::string &action
			,std::string &str_result);
		 std::vector <std::string> generateSignedHttpHeader(const std::string& action
		, const std::string& prefix
		, const std::string& body);

};