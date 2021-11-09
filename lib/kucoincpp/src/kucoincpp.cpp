#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <exception>

#include "kucoincpp.hpp"
#include "logger.hpp"
#include "magic_enum.hpp"
#include "sha.hpp"
#include "chrono.hpp"

#define KUCOIN_HOST "https://api.kucoin.com"

namespace{

size_t curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	buffer->append((char*)content, size*nmemb);
	return size*nmemb;
}

}

KucoinCPP::KucoinCPP(const std::string &api_key, const std::string &secret_key ) 
{
	LOG_INFO <<  "with api_key " << api_key;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	KucoinCPP::curl = curl_easy_init();
	KucoinCPP::api_key = api_key;
	KucoinCPP::secret_key = secret_key;
}

KucoinCPP::~KucoinCPP()
{
	LOG_INFO;
	cleanup();
}

void KucoinCPP::cleanup()
{
	LOG_INFO;
	curl_easy_cleanup(KucoinCPP::curl);
	curl_global_cleanup();
}

//Get Ticker
//{
//    "sequence": "1550467636704",
//    "bestAsk": "0.03715004",
//    "size": "0.17",
//    "price": "0.03715005",
//    "bestBidSize": "3.803",
//    "bestBid": "0.03710768",
//    "bestAskSize": "1.788",
//    "time": 1550653727731
//}
void KucoinCPP::getTicker(const std::string pairId, SpotTickersResult &json_result) const
{
    getTickersGeneric("/api/v1/market/orderbook/level1?symbol=" + pairId, json_result);
}

//Get 24hr Stats
//{
//    "time": 1602832092060,  // time
//    "symbol": "BTC-USDT",   // symbol
//    "buy": "11328.9",   // bestAsk
//    "sell": "11329",    // bestBid
//    "changeRate": "-0.0055",    // 24h change rate
//    "changePrice": "-63.6", // 24h change price
//    "high": "11610",    // 24h highest price
//    "low": "11200", // 24h lowest price
//    "vol": "2282.70993217", // 24h volume，the aggregated trading volume in BTC
//    "volValue": "25984946.157790431",   // 24h total, the trading volume in quote currency of last 24 hours
//    "last": "11328.9",  // last price
//    "averagePrice": "11360.66065903",   // 24h average transaction price yesterday
//    "takerFeeRate": "0.001",    // Basic Taker Fee
//    "makerFeeRate": "0.001",    // Basic Maker Fee
//    "takerCoefficient": "1",    // Taker Fee Coefficient
//    "makerCoefficient": "1" // Maker Fee Coefficient
//}
void KucoinCPP::get24HrStats(const std::string pairId, SpotTickersResult &json_result) const
{
    getTickersGeneric("/api/v1/market/stats?symbol=" + pairId, json_result);
}

void KucoinCPP::getTickersGeneric(const std::string url, SpotTickersResult &json_result) const
{
	LOG_DEBUG;
	CHRONO_THIS_SCOPE;

	std::string _url(KUCOIN_HOST);  
	_url += url;

	LOG_DEBUG << "url" << _url;

	std::string str_result;
	curl_api( _url, str_result );

	if ( !str_result.empty() ) 
	{	
		try 
		{
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
		} 
		catch ( std::exception &e )
		{
		 	LOG_ERROR <<  "Error ! " << e.what(); 
		}   
	}
	else
		LOG_ERROR <<  "Failed to get anything.";
}

void KucoinCPP::send_limit_order ( 
	const std::string& currency_pair, 
	const Side side,
	const TimeInForce timeInForce,
	double quantity,
	double price,
	Json::Value &json_result ) const
{
	//LOG_DEBUG;
	//CHRONO_THIS_SCOPE;

	if ( api_key.size() == 0 || secret_key.size() == 0 )
	{
		LOG_ERROR << "API Key and Secret Key has not been set.";
		return ;
	}

	std::string url(KUCOIN_HOST);
	std::string prefix("/api/v4/spot/orders");
	url += prefix;

	Json::Value bodyJson;
	bodyJson["text"] = """";
	bodyJson["currency_pair"] = currency_pair;
	bodyJson["type"] = "limit";
	bodyJson["account"] = "spot";
	bodyJson["side"] = std::string(magic_enum::enum_name(side));
	bodyJson["iceberg"] = "0";
	bodyJson["amount"] = quantity;
	bodyJson["price"] = price;
	bodyJson["time_in_force"] = std::string(magic_enum::enum_name(timeInForce));
	bodyJson["auto_borrow"] = false;

	std::string body = bodyJson.toStyledString();

	//LOG_DEBUG << "url = " << url << " body = " << body;
	
	std::string action("POST");
	const auto httpHeader = generateSignedHttpHeader(action, prefix, body);

	std::string result;
	curl_api_with_header( url, httpHeader, body, action, result ) ;
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
	} 
	else 
		LOG_ERROR << "Failed to get anything.";
}

void KucoinCPP::curl_api( std::string &url, std::string &result_json ) const
{
	curl_api_with_header( url , {}, "" , "GET", result_json );	
}

std::vector <std::string> KucoinCPP::generateSignedHttpHeader(const std::string& action, const std::string& prefix, const std::string& body) const
{
	std::string bodyHash = tools::sha512(body.c_str());
	auto timeStamp = tools::get_current_epoch();

	std::stringstream sign;
	sign << action << std::endl;
	sign << prefix << std::endl;
	sign << "" << std::endl; /* supposed to be query param */ 
	sign << bodyHash << std::endl;
	sign << timeStamp;

	std::string signHash = tools::hmac_sha512(secret_key.c_str(), sign.str().c_str());

	return std::vector <std::string>{
		"Content-Type: application/json"
		,"Timestamp: " + std::to_string(timeStamp)
		,"KEY: " + api_key
		,"SIGN: " + signHash};
}

void KucoinCPP::curl_api_with_header(const std::string &url
			,const std::vector <std::string> &extra_http_header
			,const std::string &post_data
			,const std::string &action
			, std::string &str_result) const
{
	CHRONO_THIS_SCOPE;
	CURLcode res;

	if( curl ) {

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_ENCODING, "gzip");
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(KucoinCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( action == "PUT" || action == "DELETE" ) 
			curl_easy_setopt(KucoinCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );

		if(!post_data.empty() || action == "POST")
			curl_easy_setopt(KucoinCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );

		res = curl_easy_perform(KucoinCPP::curl);
		curl_easy_reset(curl); // reset the options

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			LOG_ERROR <<  "failed: " << curl_easy_strerror(res);
		} 	
	}
}


