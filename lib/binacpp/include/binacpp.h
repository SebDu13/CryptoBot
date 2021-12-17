#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <exception>

#include <curl/curl.h>
#include "json/json.h"
#include "FixedPoint.hpp"


#define BINANCE_HOST "https://api.binance.com"

class BinaCPP 
{
	public:

	/* Names match with the api parameters, do not rename */
		enum class Side
		{
			BUY,
			SELL
		};

		/* Names match with the api parameters, do not rename */
		//GTC 	Good Til Canceled An order will be on the book unless the order is canceled.
		//IOC 	Immediate Or Cancel An order will try to fill the order as much as it can before the order expires.
		//FOK 	Fill or Kill An order will expire if the full order cannot be filled upon execution.
		enum class TimeInForce
		{
			GTC,
			IOC,
			FOK
		};

		//LIMIT 	timeInForce, quantity, price
		//MARKET 	quantity or quoteOrderQty
		//STOP_LOSS 	quantity, stopPrice
		//STOP_LOSS_LIMIT 	timeInForce, quantity, price, stopPrice
		//TAKE_PROFIT 	quantity, stopPrice
		//TAKE_PROFIT_LIMIT 	timeInForce, quantity, price, stopPrice
		//LIMIT_MAKER 	quantity, price
		enum class Type
		{
			LIMIT,
			MARKET,
			STOP_LOSS,
			STOP_LOSS_LIMIT,
			TAKE_PROFIT,
			TAKE_PROFIT_LIMIT,
			LIMIT_MAKER
		};

		BinaCPP(const std::string &api_key,const std::string &secret_key);
		virtual ~BinaCPP();
		void cleanup();
	
		// Public API
		void get_exchangeInfo( Json::Value &json_result);
		void get_serverTime( Json::Value &json_result); 	
		void get_allPrices( Json::Value &json_result );
		void get_allBookTickers( Json::Value &json_result );
		void get_depth( const std::string&symbol, int limit, Json::Value &json_result );
		void get_aggTrades( const std::string&symbol, int fromId, time_t startTime, time_t endTime, int limit, Json::Value &json_result ); 
		void get_24hr( const std::string&symbol, Json::Value &json_result ); 
		void get_klines( const std::string&symbol, const std::string&interval, int limit, time_t startTime, time_t endTime,  Json::Value &json_result );

		void getPrice(const std::string& pairId, Json::Value &json_result) const;
		void get24Ticker(const std::string& pairId, Json::Value &json_result) const;
		void getBookTicker(const std::string& pairId, Json::Value &json_result) const;


		// API + Secret keys required
		void get_account( long recvWindow , Json::Value &json_result );
		
		void get_myTrades( 
			const std::string&symbol, 
			int limit,
			long fromId,
			long recvWindow, 
			Json::Value &json_result 
		);
		
		void get_openOrders(  
			const std::string&symbol, 
			long recvWindow,   
			Json::Value &json_result 
		) ;
		

		void get_allOrders(   
			const std::string&symbol, 
			long orderId,
			int limit,
			long recvWindow,
			Json::Value &json_result 
		);
		 
		void send_order( 
			const std::string&symbol, 
			const Side side,
			const Type type,
			const TimeInForce timeInForce,
			Quantity quantity,
			Price price,
			double stopPrice,
			double icebergQty,
			long recvWindow,
			Json::Value &json_result ) const;
		
		void send_order( 
			const std::string& symbol, 
			const Side side,
			const Type type,
			const TimeInForce timeInForce,
			Quantity quantity,
			Price price,
			Json::Value &json_result ) const;

		void get_order( 
			const std::string&symbol, 
			long orderId,
			const std::string&origClientOrderId,
			long recvWindow,
			Json::Value &json_result ); 


		void cancel_order( 
			const std::string&symbol, 
			long orderId,
			const std::string&origClientOrderId,
			const std::string&newClientOrderId,
			long recvWindow,
			Json::Value &json_result 
		);

		// API key required
		void start_userDataStream( Json::Value &json_result );
		void keep_userDataStream( const std::string&listenKey  );
		void close_userDataStream( const std::string&listenKey );


		// WAPI
		void withdraw( 
			const std::string&asset,
			const std::string&address,
			const std::string&addressTag,
			double amount, 
			const std::string&name,
			long recvWindow,
			Json::Value &json_result );

		void get_depositHistory( 
			const std::string&asset,
			int  status,
			long startTime,
			long endTime, 
			long recvWindow,
			Json::Value &json_result );

		void get_withdrawHistory( 
			const std::string&asset,
			int  status,
			long startTime,
			long endTime, 
			long recvWindow,
			Json::Value &json_result ); 

		void get_depositAddress( 
			const std::string&asset,
			long recvWindow,
			Json::Value &json_result );

	private:
		std::string api_key;
		std::string secret_key;
		CURL* curl;

		void curl_api( std::string &url, std::string &result_json ) const;
		void curl_api_with_header( std::string &url, std::string &result_json , std::vector <std::string> &extra_http_header, std::string &post_data, std::string &action ) const;
		void getTickersGeneric(const std::string& url, Json::Value &json_result) const;
};
