#pragma once

#include <string.h>

#define GATEIO_HOST "https://api.gateio.ws"


class GateIoCPP {
	static std::string api_key;
	static std::string secret_key;
	static CURL* curl;

	public:

		static void curl_api( std::string &url, std::string &result_json );
		static void curl_api_with_header( std::string &url
			, std::string &result_json 
			, std::vector <std::string> &extra_http_header
			, std::string &post_data
			, std::string &action );
		static size_t curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) ;
		
		static void init( std::string &api_key, std::string &secret_key);
		static void cleanup();


		// Public API
		static void get_currency_pairs( Json::Value &json_result);
};