#pragma once

#include <string>

#define GATEIO_HOST "https://api.gateio.ws"


class GateIoCPP {
	public:
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

		static void init( std::string &api_key, std::string &secret_key);
		static void cleanup();

		// Public API
		static void get_currency_pairs( Json::Value &json_result);
		static void send_limit_order( 
			const std::string_view& currency_pair, 
			const Side side,
			const TimeInForce timeInForce,
			double quantity,
			double price,
			Json::Value &json_result );

		private:
		inline static std::string api_key = "";
		inline static std::string secret_key = "";
		inline static CURL* curl = NULL;

		static size_t curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) ;
		static void curl_api( std::string &url, std::string &result_json );
		static void curl_api_with_header(const std::string &url
			,const std::vector <std::string> &extra_http_header
			,const std::string &post_data
			,const std::string &action
			,std::string &str_result); 

};