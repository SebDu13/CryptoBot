#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <exception>

#include <curl/curl.h>
#include "json/json.h"
#include "gateiocpp.h"
#include "logger.hpp"
#include "magic_enum.hpp"
#include "sha.hpp"

//---------------------------------
void GateIoCPP::init( std::string &api_key, std::string &secret_key ) 
{
	LOG_INFO <<  "with api_key " << api_key;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	GateIoCPP::curl = curl_easy_init();
	GateIoCPP::api_key = api_key;
	GateIoCPP::secret_key = secret_key;
}


void GateIoCPP::cleanup()
{
	LOG_INFO;
	curl_easy_cleanup(GateIoCPP::curl);
	curl_global_cleanup();
}

//------------------
//GET api/v1/exchangeInfo
//------------------
void GateIoCPP::get_currency_pairs( Json::Value &json_result)
{
	LOG_DEBUG;

	std::string url(GATEIO_HOST);  
	url += "/api/v4/spot/currency_pairs";

	std::string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( std::exception &e ) {
		 	LOG_ERROR <<  "Error ! " << e.what(); 
		}   
		LOG_DEBUG <<  "Done.";
	
	} else {
		LOG_ERROR <<  "Failed to get anything.";
	}
}

void GateIoCPP::send_limit_order( 
	const std::string_view& currency_pair, 
	const Side side,
	const TimeInForce timeInForce,
	double quantity,
	double price,
	Json::Value &json_result )
{
	LOG_DEBUG;

	if ( api_key.size() == 0 || secret_key.size() == 0 )
	{
		LOG_ERROR << "API Key and Secret Key has not been set.";
		return ;
	}

	std::string url(GATEIO_HOST);
	std::string prefix("/api/v4/spot/orders");
	
	url += prefix;

	std::string action("POST");

	//body='{"text":"t-123456","currency_pair":"ETH_BTC","type":"limit","account":"spot"
	//,"side":"buy","iceberg":"0","amount":"1","price":"5.00032","time_in_force":"gtc","auto_borrow":false}'
	
	std::stringstream post_data;
	post_data << "currency_pair=" << currency_pair;
	post_data << ",type=limit"; /* only limit order available in gateio api */
	post_data << ",account=spot";
	post_data << ",side=" << magic_enum::enum_name(side);
	post_data << ",iceberg=0";
	post_data << ",amount=" << quantity;
	post_data << ",price=" << price;
	post_data << ",time_in_force=" << magic_enum::enum_name(timeInForce);

	std::string bodyHash = tools::sha512(post_data.str().c_str());

	auto timeStamp = tools::get_current_epoch();
	std::stringstream sign;
	sign << action << std::endl;
	sign << prefix << std::endl;
	sign << "" << std::endl; /* supposed to be query param */ 
	sign << bodyHash << std::endl;
	sign << timeStamp;

	std::string signHash = tools::hmac_sha512(secret_key.c_str(), sign.str().c_str());
	std::vector <std::string> extra_http_header{
		"Content-Type: application/json"
		,"Timestamp: " + std::to_string(timeStamp)
		,"KEY: " + api_key
		,"SIGN: " + signHash};


	LOG_DEBUG << "url = " << url << " post_data = " << post_data.str();
	
	std::string result;
	curl_api_with_header( url, extra_http_header, post_data.str(), action, result ) ;

	if ( result.size() > 0 ) 
	{
		try 
		{
			Json::Reader reader;
			json_result.clear();	
			reader.parse( result , json_result );
	    		
	    } 
		catch ( std::exception &e ) 
		{
		 	LOG_ERROR << " Error ! " << e.what(); 
		}   
		LOG_DEBUG << "Done.";
	} 
	else 
		LOG_ERROR << "Failed to get anything.";
	
	LOG_INFO << "Done.\n";
}


size_t GateIoCPP::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	LOG_DEBUG;

	buffer->append((char*)content, size*nmemb);

	LOG_DEBUG << "done";
	return size*nmemb;
}

void GateIoCPP::curl_api( std::string &url, std::string &result_json ) {
	std::vector <std::string> v;
	std::string action = "GET";
	std::string post_data = "";
	curl_api_with_header( url , v, post_data , action, result_json );	
} 

void GateIoCPP::curl_api_with_header(const std::string &url
			,const std::vector <std::string> &extra_http_header
			,const std::string &post_data
			,const std::string &action
			, std::string &str_result)
{
	LOG_DEBUG;

	CURLcode res;

	if( GateIoCPP::curl ) {

		curl_easy_setopt(GateIoCPP::curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(GateIoCPP::curl, CURLOPT_WRITEFUNCTION, GateIoCPP::curl_cb);
		curl_easy_setopt(GateIoCPP::curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(GateIoCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(GateIoCPP::curl, CURLOPT_ENCODING, "gzip");

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(GateIoCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(GateIoCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			curl_easy_setopt(GateIoCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(GateIoCPP::curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			LOG_ERROR <<  "failed: " << curl_easy_strerror(res);
		} 	

	}

	LOG_DEBUG <<  "done";

}



