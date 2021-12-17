
#include "binacpp.h"
#include "logger.hpp"
#include "binacpp_utils.h"
#include "chrono.hpp"
#include "magic_enum.hpp"

namespace
{

size_t curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	buffer->append((char*)content, size*nmemb);
	return size*nmemb;
}

void convertToJson(const std::string& input, Json::Value& output)
{
	try 
	{
		Json::Reader reader;
		output.clear();	
		reader.parse( input , output );
	} 
	catch ( std::exception &e ) 
	{
		LOG_ERROR <<  "Error ! " << e.what(); 
	}
}

}

BinaCPP::BinaCPP(const std::string &api_key,const std::string &secret_key)
: curl(curl_easy_init())
, api_key(api_key)
, secret_key(secret_key)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

BinaCPP::~BinaCPP()
{
	cleanup();
}

void BinaCPP::cleanup()
{
	curl_easy_cleanup(BinaCPP::curl);
	curl_global_cleanup();
}

void BinaCPP::getTickersGeneric(const std::string& url, Json::Value &json_result) const
{
	std::string _url(BINANCE_HOST);  
	_url += url;

	//LOG_DEBUG << _url;

	std::string str_result;
	curl_api( _url, str_result );

	if ( !str_result.empty() ) 
	{	
		convertToJson(str_result, json_result);
	}
	else
		LOG_ERROR <<  "Failed to get anything.";
}

//------------------
//GET api/v1/exchangeInfo
//------------------
void BinaCPP::get_exchangeInfo( Json::Value &json_result)
{
	CHRONO_THIS_SCOPE;

	LOG_INFO << "<BinaCPP::get_exchangeInfo>";

	string url(BINANCE_HOST);  
	url += "/api/v1/exchangeInfo";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_exchangeInfo> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_exchangeInfo> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_exchangeInfo> Failed to get anything.";
	}
}
//------------------
//GET /api/v1/time
//------------
void BinaCPP::get_serverTime( Json::Value &json_result) 
{
	LOG_INFO << "<BinaCPP::get_serverTime>";

	string url(BINANCE_HOST);  
	url += "/api/v1/time";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_serverTime> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_serverTime> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_serverTime> Failed to get anything.";
	}
}



//--------------------
// Get Latest price for all symbols.
/*
	GET /api/v1/ticker/allPrices
*/
void BinaCPP::get_allPrices( Json::Value &json_result ) 
{	

	LOG_INFO << "<BinaCPP::get_allPrices>";

	string url(BINANCE_HOST);  
	url += "/api/v1/ticker/allPrices";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_allPrices> Error ! %s", e.what() ; 
		}   
		LOG_INFO << "<BinaCPP::get_allPrices> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_allPrices> Failed to get anything.";
	}
}

void BinaCPP::get24Ticker(const std::string& pairId, Json::Value &json_result) const
{
    getTickersGeneric("/api/v3/ticker/24hr?symbol=" + pairId, json_result);
}

void BinaCPP::getPrice(const std::string& pairId, Json::Value &json_result) const
{
    getTickersGeneric("/api/v3/ticker/price?symbol=" + pairId, json_result);
}

void BinaCPP::getBookTicker(const std::string& pairId, Json::Value &json_result) const
{
    getTickersGeneric("/api/v3/ticker/bookTicker?symbol=" + pairId, json_result);
}


//--------------------
// Get Best price/qty on the order book for all symbols.
/*
	GET /api/v1/ticker/allBookTickers
	
*/

void 
BinaCPP::get_allBookTickers(  Json::Value &json_result ) 
{	

	LOG_INFO <<"<BinaCPP::get_allBookTickers>";

	string url(BINANCE_HOST);  
	url += "/api/v1/ticker/allBookTickers";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_allBookTickers> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_allBookTickers> Done.";
	
	} else {
		LOG_INFO <<"<BinaCPP::get_allBookTickers> Failed to get anything.";
	}
}



//--------------------
// Get Market Depth
/*
GET /api/v1/depth

Name	Type		Mandatory	Description
symbol	STRING		YES	
limit	INT		NO		Default 100; max 100.

*/

void 
BinaCPP::get_depth( 
	const std::string&symbol, 
	int limit, 
	Json::Value &json_result ) 
{	

	LOG_INFO <<"<BinaCPP::get_depth>";

	string url(BINANCE_HOST);  
	url += "/api/v1/depth?";

	string querystring("symbol=");
	querystring.append( symbol );
	querystring.append("&limit=");
	querystring.append( to_string( limit ) );

	url.append( querystring );
	LOG_INFO << "<BinaCPP::get_depth> url = |%s|" , url.c_str();
	
	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_depth> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_depth> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_depth> Failed to get anything.";
	}
}







//--------------------
// Get Aggregated Trades list
/*

GET /api/v1/aggTrades

Name		Type	Mandatory	Description
symbol		STRING	YES	
fromId		LONG	NO		ID to get aggregate trades from INCLUSIVE.
startTime	LONG	NO		Timestamp in ms to get aggregate trades from INCLUSIVE.
endTime		LONG	NO		Timestamp in ms to get aggregate trades until INCLUSIVE.
limit		INT	NO		Default 500; max 500.
*/

void 
BinaCPP::get_aggTrades( 
	const std::string&symbol, 
	int fromId, 
	time_t startTime, 
	time_t endTime, 
	int limit, 
	Json::Value &json_result 
) 
{	

	LOG_INFO <<"<BinaCPP::get_aggTrades>";

	string url(BINANCE_HOST);  
	url += "/api/v1/aggTrades?";

	string querystring("symbol=");
	querystring.append( symbol );

	
	if ( startTime != 0 && endTime != 0 ) {

		querystring.append("&startTime=");
		querystring.append( to_string( startTime ) );

		querystring.append("&endTime=");
		querystring.append( to_string( endTime ) );
	
	} else {
		querystring.append("&fromId=");
		querystring.append( to_string( fromId ) );

		querystring.append("&limit=");
		querystring.append( to_string( limit ) );
	}

	url.append( querystring );
	LOG_INFO << "<BinaCPP::get_aggTrades> url = |%s|" , url.c_str();
	
	string str_result;
	curl_api( url, str_result ) ;
	
	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_aggTrades> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_aggTrades> Done." ;
	
	} else {
		LOG_INFO << "<BinaCPP::get_aggTrades> Failed to get anything.";
	}
}









//--------------------
// Get 24hr ticker price change statistics
/*
Name	Type	Mandatory	Description
symbol	STRING	YES	
*/
void 
BinaCPP::get_24hr( const std::string&symbol, Json::Value &json_result ) 
{	

	LOG_INFO << "<BinaCPP::get_24hr>";

	string url(BINANCE_HOST);  
	url += "/api/v1/ticker/24hr?";

	string querystring("symbol=");
	querystring.append( symbol );


	
	url.append( querystring );
	LOG_INFO << "<BinaCPP::get_24hr> url = |%s|" , url.c_str();
	
	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
	    		reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_24hr> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_24hr> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_24hr> Failed to get anything.";
	}
}





//-----------------
/*

Get KLines( Candle stick / OHLC )
GET /api/v1/klines

Name		Type	Mandatory	Description
symbol		STRING	YES	
interval	ENUM	YES	
limit		INT		NO	Default 500; max 500.
startTime	LONG	NO	
endTime		LONG	NO	

*/

void 
BinaCPP::get_klines( 
	const std::string&symbol, 
	const std::string&interval, 
	int limit, 
	time_t startTime, 
	time_t endTime,  
	Json::Value &json_result ) 
{		

	LOG_INFO << "<BinaCPP::get_klines>";

	string url(BINANCE_HOST);  
	url += "/api/v1/klines?";

	string querystring("symbol=");
	querystring.append( symbol );

	querystring.append( "&interval=" );
	querystring.append( interval );

	if ( startTime > 0 && endTime > 0 ) {

		querystring.append("&startTime=");
		querystring.append( to_string( startTime ) );

		querystring.append("&endTime=");
		querystring.append( to_string( endTime ) );
	
	} else if ( limit > 0 ) {
		querystring.append("&limit=");
		querystring.append( to_string( limit ) );
	}

	
	url.append( querystring );
	LOG_INFO << "<BinaCPP::get_klines> url = |%s|" , url.c_str();
	
	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
	    		json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_klines> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_klines> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_klines> Failed to get anything.";
	}
}














//--------------------
// Get current account information. (SIGNED)
/*
GET /api/v3/account

Parameters:
Name		Type	Mandatory	Description
recvWindow	LONG	NO	
timestamp	LONG	YES
*/


void 
BinaCPP::get_account( long recvWindow,  Json::Value &json_result ) 
{	

	LOG_INFO << "<BinaCPP::get_account>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_account> API Key and Secret Key has not been set.";
		return ;
	}


	string url(BINANCE_HOST);
	url += "/api/v3/account?";
	string action = "GET";
	

	string querystring("timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow ) );
	}

	string signature =  hmac_sha256( secret_key.c_str() , querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	LOG_INFO << "<BinaCPP::get_account> url = |%s|" , url.c_str();
	
	string post_data = "";
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_account> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_account> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_account> Failed to get anything.";
	}

	LOG_INFO << "<BinaCPP::get_account> Done.\n";

}








//--------------------
// Get trades for a specific account and symbol. (SIGNED)
/*
GET /api/v3/myTrades
Name		Type	Mandatory	Description
symbol		STRING	YES	
limit		INT		NO	Default 500; max 500.
fromId		LONG	NO	TradeId to fetch from. Default gets most recent trades.
recvWindow	LONG	NO	
timestamp	LONG	YES
	
*/


void 
BinaCPP::get_myTrades( 
	const std::string&symbol,
	int limit,
	long fromId,
	long recvWindow, 
	Json::Value &json_result ) 
{	

	LOG_INFO << "<BinaCPP::get_myTrades>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_myTrades> API Key and Secret Key has not been set.";
		return ;
	}


	string url(BINANCE_HOST);
	url += "/api/v3/myTrades?";

	string querystring("symbol=");
	querystring.append( symbol );

	if ( limit > 0 ) {
		querystring.append("&limit=");
		querystring.append( to_string( limit ) );
	}

	if ( fromId > 0 ) {
		querystring.append("&fromId=");
		querystring.append( to_string( fromId ) );
	}

	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow ) );
	}

	querystring.append("&timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str() , querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	LOG_INFO << "<BinaCPP::get_myTrades> url = |%s|" , url.c_str();
	
	string action = "GET";
	string post_data = "";

	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_myTrades> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_myTrades> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_myTrades> Failed to get anything.";
	}

	LOG_INFO << "<BinaCPP::get_myTrades> Done.\n";

}











//--------------------
// Open Orders (SIGNED)
/*
GET /api/v3/openOrders

Name		Type	Mandatory	Description
symbol		STRING	YES	
recvWindow	LONG	NO	
timestamp	LONG	YES	
*/

void 
BinaCPP::get_openOrders( 
	const std::string&symbol, 
	long recvWindow,  
	Json::Value &json_result 
) 
{	

	LOG_INFO << "<BinaCPP::get_openOrders>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_openOrders> API Key and Secret Key has not been set.";
		return ;
	}


	string url(BINANCE_HOST);
	url += "/api/v3/openOrders?";

	string querystring("symbol=");
	querystring.append( symbol );

	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow) );
	}


	querystring.append("&timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );


	string signature =  hmac_sha256( secret_key.c_str(), querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	
	string action = "GET";
	string post_data ="";
		
	LOG_INFO << "<BinaCPP::get_openOrders> url = |%s|" , url.c_str();
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_openOrders> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_openOrders> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_openOrders> Failed to get anything.";
	}
	
	LOG_INFO << "<BinaCPP::get_openOrders> Done.\n";

}



//--------------------
// All Orders (SIGNED)
/*
GET /api/v3/allOrders

Name		Type	Mandatory	Description
symbol		STRING	YES	
orderId		LONG	NO	
limit		INT		NO		Default 500; max 500.
recvWindow	LONG	NO	
timestamp	LONG	YES	
*/

void BinaCPP::get_allOrders( 
	const std::string&symbol, 
	long orderId,
	int limit,
	long recvWindow,
	Json::Value &json_result 
) 

{	

	LOG_INFO << "<BinaCPP::get_allOrders>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_allOrders> API Key and Secret Key has not been set.";
		return ;
	}


	string url(BINANCE_HOST);
	url += "/api/v3/allOrders?";

	string querystring("symbol=");
	querystring.append( symbol );

	if ( orderId > 0 ) {
		querystring.append("&orderId=");
		querystring.append( to_string( orderId ) );
	}

	if ( limit > 0 ) {
		querystring.append("&limit=");
		querystring.append( to_string( limit ) );
	}

	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow ) );
	}


	querystring.append("&timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str(), querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	
	string action = "GET";
	string post_data ="";
		
	LOG_INFO << "<BinaCPP::get_allOrders> url = |%s|" , url.c_str();
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data , action ) ;


	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_allOrders> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_allOrders> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_allOrders> Failed to get anything.";
	}
	
	LOG_INFO << "<BinaCPP::get_allOrders> Done.\n";

}

void BinaCPP::send_order( 
	const std::string& symbol, 
	const Side side,
	const Type type,
	const TimeInForce timeInForce,
	Quantity quantity,
	Price price,
	Json::Value &json_result ) const
{
	BinaCPP::send_order( 
	symbol, 
	side,
	type,
	timeInForce,
	quantity,
	price,
	0,
	0,
	0,
	json_result);
}

//------------
/*
send order (SIGNED)
POST /api/v3/order

Name				Type		Mandatory	Description
symbol				STRING		YES	
side				ENUM		YES	
type				ENUM		YES	
timeInForce			ENUM		YES	
quantity			DECIMAL		YES	
price				DECIMAL		YES	
newClientOrderId		STRING		NO		A unique id for the order. Automatically generated by default.
stopPrice			DECIMAL		NO		Used with STOP orders
icebergQty			DECIMAL		NO		Used with icebergOrders
recvWindow			LONG		NO	
timestamp			LONG		YES	
*/

void 
BinaCPP::send_order( 
	const std::string& symbol, 
	const Side side,
	const Type type,
	const TimeInForce timeInForce,
	Quantity quantity,
	Price price,
	double stopPrice,
	double icebergQty,
	long recvWindow,
	Json::Value &json_result ) const
{	

	//CHRONO_THIS_SCOPE;

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::send_order> API Key and Secret Key has not been set.";
		return ;
	}

	string url(BINANCE_HOST);
	url += "/api/v3/order?";

	string action = "POST";
	
	string post_data("symbol=");
	post_data.append( symbol );
	
	post_data.append("&side=");
	post_data.append( magic_enum::enum_name(side) );

	post_data.append("&type=");
	post_data.append( magic_enum::enum_name(type) );
	if(type != Type::MARKET)
	{
		post_data.append("&timeInForce=");
		post_data.append( magic_enum::enum_name(timeInForce));
		post_data.append("&price=");
		post_data.append( price.toString() );
		post_data.append("&quantity=");
		post_data.append( quantity.toString() );
	}
	else
	{
		if(side == Side::SELL)
		{
			post_data.append("&quantity=");
			post_data.append( quantity.toString(2) ); // recuperer LOT_SIZE
		}
		else if(side == Side::BUY)
		{
			post_data.append("&quoteOrderQty=");
			post_data.append( quantity.toString() );
		}
		//post_data.append("&quoteOrderQty=");
		//post_data.append( quantity.toString() );
	}

	
	if ( stopPrice > 0.0 ) {
		post_data.append("&stopPrice=");
		post_data.append( to_string( stopPrice ) );
	}

	if ( icebergQty > 0.0 ) {
		post_data.append("&icebergQty=");
		post_data.append( to_string( icebergQty ) );
	}

	if ( recvWindow > 0 ) {
		post_data.append("&recvWindow=");
		post_data.append( to_string( recvWindow) );
	}


	post_data.append("&timestamp=");
	post_data.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str(), post_data.c_str() );
	post_data.append( "&signature=");
	post_data.append( signature );

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	LOG_INFO << "<BinaCPP::send_order> url = " << url << " post_data = " << post_data.c_str();
	
	LOG_DEBUG;
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data, action ) ;

	if ( str_result.size() > 0 ) 
	{
		convertToJson(str_result, json_result); 
	} 
	else 
		LOG_ERROR << "Failed to get anything.";

}


//------------------
/*
// get order (SIGNED)
GET /api/v3/order

Name				Type	Mandatory	Description
symbol				STRING	YES	
orderId				LONG	NO	
origClientOrderId		STRING	NO	
recvWindow			LONG	NO	
timestamp			LONG	YES	
*/

void 
BinaCPP::get_order( 
	const std::string& symbol, 
	long orderId,
	const std::string& origClientOrderId,
	long recvWindow,
	Json::Value& json_result ) 
{	

	LOG_INFO << "<BinaCPP::get_order>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_order> API Key and Secret Key has not been set.";
		return ;
	}

	string url(BINANCE_HOST);
	url += "/api/v3/order?";
	string action = "GET";
	

	string querystring("symbol=");
	querystring.append( symbol );
	
	if ( orderId > 0 ) {
		querystring.append("&orderId=");
		querystring.append( to_string( orderId ) );
	}

	if ( !origClientOrderId.empty()) {
		querystring.append("&origClientOrderId=");
		querystring.append( origClientOrderId );
	}

	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow) );
	}

	querystring.append("&timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str(), querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string post_data = "";
	
	LOG_INFO << "<BinaCPP::get_order> url = |%s|" , url.c_str();
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_order> Error ! " << e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_order> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_order> Failed to get anything.";
	}

	
	LOG_INFO << "<BinaCPP::get_order> Done.\n";

}








//------------
/*
DELETE /api/v3/order
cancel order (SIGNED)

symbol				STRING	YES	
orderId				LONG	NO	
origClientOrderId		STRING	NO	
newClientOrderId		STRING	NO	Used to uniquely identify this cancel. Automatically generated by default.
recvWindow			LONG	NO	
timestamp			LONG	YES	

*/

void 
BinaCPP::cancel_order( 
	const std::string& symbol, 
	long orderId,
	const std::string& origClientOrderId,
	const std::string& newClientOrderId,
	long recvWindow,
	Json::Value &json_result ) 
{	

	LOG_INFO << "<BinaCPP::send_order>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::send_order> API Key and Secret Key has not been set.";
		return ;
	}

	string url(BINANCE_HOST);
	url += "/api/v3/order?";

	string action = "DELETE";
	
	string post_data("symbol=");
	post_data.append( symbol );

	if ( orderId > 0 ) {	
		post_data.append("&orderId=");
		post_data.append( to_string( orderId ) );
	}

	if ( !origClientOrderId.empty() ) 
	{
		post_data.append("&origClientOrderId=");
		post_data.append( origClientOrderId );
	}

	if ( !newClientOrderId.empty() ) 
	{
		post_data.append("&newClientOrderId=");
		post_data.append( newClientOrderId );
	}

	if ( recvWindow > 0 ) {
		post_data.append("&recvWindow=");
		post_data.append( to_string( recvWindow) );
	}


	post_data.append("&timestamp=");
	post_data.append( to_string( get_current_ms_epoch() ) );


	string signature =  hmac_sha256( secret_key.c_str(), post_data.c_str() );
	post_data.append( "&signature=");
	post_data.append( signature );


	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	LOG_INFO << "<BinaCPP::send_order> url = |%s|, post_data = |%s|" , url.c_str(), post_data.c_str();
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data, action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::send_order> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::send_order> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::send_order> Failed to get anything.";
	}
	
	LOG_INFO << "<BinaCPP::send_order> Done.\n";

}





//--------------------
//Start user data stream (API-KEY)

void 
BinaCPP::start_userDataStream( Json::Value &json_result ) 
{	
	LOG_INFO << "<BinaCPP::start_userDataStream>";

	if ( api_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::start_userDataStream> API Key has not been set.";
		return ;
	}


	string url(BINANCE_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	LOG_INFO << "<BinaCPP::start_userDataStream> url = |%s|" , url.c_str();
	
	string action = "POST";
	string post_data = "";

	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::start_userDataStream> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::start_userDataStream> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::start_userDataStream> Failed to get anything.";
	}

	LOG_INFO << "<BinaCPP::start_userDataStream> Done.\n";

}









//--------------------
//Keepalive user data stream (API-KEY)
void 
BinaCPP::keep_userDataStream( const std::string& listenKey ) 
{	
	LOG_INFO << "<BinaCPP::keep_userDataStream>";

	if ( api_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::keep_userDataStream> API Key has not been set.";
		return ;
	}


	string url(BINANCE_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "PUT";
	string post_data("listenKey=");
	post_data.append( listenKey );

	LOG_INFO << "<BinaCPP::keep_userDataStream> url = " << url << "," << " post_data = " << post_data;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		LOG_INFO << "<BinaCPP::keep_userDataStream> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::keep_userDataStream> Failed to get anything.";
	}

	LOG_INFO << "<BinaCPP::keep_userDataStream> Done.\n";

}





//--------------------
//Keepalive user data stream (API-KEY)
void 
BinaCPP::close_userDataStream( const std::string& listenKey ) 
{	
	LOG_INFO << "<BinaCPP::close_userDataStream>";

	if ( api_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::close_userDataStream> API Key has not been set.";
		return ;
	}


	string url(BINANCE_HOST);
	url += "/api/v1/userDataStream";

	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	

	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string action = "DELETE";
	string post_data("listenKey=");
	post_data.append( listenKey );

	LOG_INFO << "<BinaCPP::close_userDataStream> url = " <<url << " ," << " post_data = " << post_data;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header , post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		LOG_INFO << "<BinaCPP::close_userDataStream> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::close_userDataStream> Failed to get anything.";
	}

	LOG_INFO << "<BinaCPP::close_userDataStream> Done.\n";

}




//-------------
/*
Submit a withdraw request.
 
POST /wapi/v3/withdraw.html

Name		Type	Mandatory	Description
asset		STRING	YES	
address		STRING	YES	
addressTag	STRING	NO	Secondary address identifier for coins like XRP,XMR etc.
amount		DECIMAL	YES	
name		STRING	NO	Description of the address
recvWindow	LONG	NO	
timestamp	LONG	YES

*/
void 
BinaCPP::withdraw( 
	const std::string& asset,
	const std::string& address,
	const std::string& addressTag,
	double amount, 
	const std::string& name,
	long recvWindow,
	Json::Value &json_result ) 
{	

	LOG_INFO << "<BinaCPP::withdraw>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::send_order> API Key and Secret Key has not been set.";
		return ;
	}

	string url(BINANCE_HOST);
	url += "/wapi/v3/withdraw.html";

	string action = "POST";
	
	string post_data("asset=");
	post_data.append( asset );
	
	post_data.append("&address=" );
	post_data.append( address );

	if ( !addressTag.empty()) {
		post_data.append( "&addressTag=");
		post_data.append( addressTag );
	}

	post_data.append( "&amount=");
	post_data.append( to_string( amount ));	

	if ( !name.empty() ) {
		post_data.append("&name=");
		post_data.append(name);
	}

	if ( recvWindow > 0 ) {
		post_data.append("&recvWindow=");
		post_data.append( to_string( recvWindow) );
	}

	post_data.append("&timestamp=");
	post_data.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str(), post_data.c_str() );
	post_data.append( "&signature=");
	post_data.append( signature );


	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	LOG_INFO << "<BinaCPP::withdraw> url = " << url << " , post_data = " << post_data;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data, action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::withdraw> Error ! %s", e.what(); 
		}   
		LOG_INFO << "<BinaCPP::withdraw> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::withdraw> Failed to get anything.";
	}
	
	LOG_INFO << "<BinaCPP::withdraw> Done.\n";

}



/*
-GET /wapi/v3/depositHistory.html
Fetch deposit history.

Parameters:

Name		Type	Mandatory	Description
asset		STRING	NO	
status		INT	NO	0(0:pending,1:success)
startTime	LONG	NO	
endTime	LONG		NO	
recvWindow	LONG	NO	
timestamp	LONG	YES	
*/
void 
BinaCPP::get_depositHistory( 
	const std::string& asset,
	int  status,
	long startTime,
	long endTime, 
	long recvWindow,
	Json::Value &json_result ) 
{	


	LOG_INFO << "<BinaCPP::get_depostHistory>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_depostHistory> API Key and Secret Key has not been set.";
		return ;
	}

	string url(BINANCE_HOST);
	url += "/wapi/v3/depositHistory.html?";
	string action = "GET";
	
	string querystring("");

	if ( !asset.empty() ) {
		querystring.append( "asset=" );
		querystring.append( asset );
	}

	if ( status > 0 ) {
		querystring.append("&status=");
		querystring.append( to_string( status ) );
	}

	if ( startTime > 0 ) {
		querystring.append("&startTime=");
		querystring.append( to_string( startTime ) );
	}

	if ( endTime > 0 ) {
		querystring.append("&endTime=");
		querystring.append( to_string( endTime ) );
	}

	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow) );
	}

	querystring.append("&timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str(), querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string post_data = "";
	
	LOG_INFO << "<BinaCPP::get_depostHistory> url = " << url;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_depostHistory> Error ! " <<  e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_depostHistory> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_depostHistory> Failed to get anything.";
	}
	
	LOG_INFO << "<BinaCPP::get_depostHistory> Done.\n";
}










//---------

/*
-GET /wapi/v3/withdrawHistory.html
Fetch withdraw history.

Parameters:

Name		Type	Mandatory	Description
asset		STRING	NO	
status		INT	NO	0(0:Email Sent,1:Cancelled 2:Awaiting Approval 3:Rejected 4:Processing 5:Failure 6Completed)
startTime	LONG	NO	
endTime	LONG		NO	
recvWindow	LONG	NO	
timestamp	LONG	YES	
*/

void 
BinaCPP::get_withdrawHistory( 
	const std::string& asset,
	int  status,
	long startTime,
	long endTime, 
	long recvWindow,
	Json::Value &json_result ) 
{	


	LOG_INFO << "<BinaCPP::get_withdrawHistory>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_withdrawHistory> API Key and Secret Key has not been set.";
		return ;
	}

	string url(BINANCE_HOST);
	url += "/wapi/v3/withdrawHistory.html?";
	string action = "GET";
	
	string querystring("");

	if ( !asset.empty() ) {
		querystring.append( "asset=" );
		querystring.append( asset );
	}

	if ( status > 0 ) {
		querystring.append("&status=");
		querystring.append( to_string( status ) );
	}

	if ( startTime > 0 ) {
		querystring.append("&startTime=");
		querystring.append( to_string( startTime ) );
	}

	if ( endTime > 0 ) {
		querystring.append("&endTime=");
		querystring.append( to_string( endTime ) );
	}

	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow) );
	}

	querystring.append("&timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str(), querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string post_data = "";
	
	LOG_INFO << "<BinaCPP::get_withdrawHistory> url = " << url;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_withdrawHistory> Error ! " << e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_withdrawHistory> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_withdrawHistory> Failed to get anything.";
	}
	
	LOG_INFO << "<BinaCPP::get_withdrawHistory> Done.\n";
}






//--------------
/*
-GET /wapi/v3/depositAddress.html
Fetch deposit address.

Parameters:

Name		Type	Mandatory	Description
asset		STRING	YES	
recvWindow	LONG	NO	
timestamp	LONG	YES	

*/


void 
BinaCPP::get_depositAddress( 
	const std::string& asset,
	long recvWindow,
	Json::Value &json_result ) 
{	


	LOG_INFO << "<BinaCPP::get_depositAddress>";

	if ( api_key.size() == 0 || secret_key.size() == 0 ) {
		LOG_INFO << "<BinaCPP::get_depositAddress> API Key and Secret Key has not been set.";
		return ;
	}

	string url(BINANCE_HOST);
	url += "/wapi/v3/depositAddress.html?";
	string action = "GET";
	
	string querystring("asset=");
	querystring.append( asset );
	
	if ( recvWindow > 0 ) {
		querystring.append("&recvWindow=");
		querystring.append( to_string( recvWindow) );
	}

	querystring.append("&timestamp=");
	querystring.append( to_string( get_current_ms_epoch() ) );

	string signature =  hmac_sha256( secret_key.c_str(), querystring.c_str() );
	querystring.append( "&signature=");
	querystring.append( signature );

	url.append( querystring );
	
	vector <string> extra_http_header;
	string header_chunk("X-MBX-APIKEY: ");
	header_chunk.append( api_key );
	extra_http_header.push_back(header_chunk);

	string post_data = "";
	
	LOG_INFO << "<BinaCPP::get_depositAddress> url = " << url;
	
	string str_result;
	curl_api_with_header( url, str_result , extra_http_header, post_data , action ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
	    	} catch ( exception &e ) {
		 	LOG_INFO << "<BinaCPP::get_depositAddress> Error ! " << e.what(); 
		}   
		LOG_INFO << "<BinaCPP::get_depositAddress> Done.";
	
	} else {
		LOG_INFO << "<BinaCPP::get_depositAddress> Failed to get anything.";
	}
	
	LOG_INFO << "<BinaCPP::get_depositAddress> Done.\n";
}


//--------------------------------------------------
void 
BinaCPP::curl_api( string &url, string &result_json ) const 
{
	vector <string> v;
	string action = "GET";
	string post_data = "";
	curl_api_with_header( url , result_json , v, post_data , action );	
} 



//--------------------
// Do the curl
void 
BinaCPP::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action ) const 
{
	CURLcode res;

	if( BinaCPP::curl ) {

		curl_easy_setopt(BinaCPP::curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(BinaCPP::curl, CURLOPT_WRITEFUNCTION, curl_cb);
		curl_easy_setopt(BinaCPP::curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(BinaCPP::curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(BinaCPP::curl, CURLOPT_ENCODING, "gzip");
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) {
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(BinaCPP::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( post_data.size() > 0 || action == "POST" || action == "PUT" || action == "DELETE" ) {

			if ( action == "PUT" || action == "DELETE" ) {
				curl_easy_setopt(BinaCPP::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );
			}
			curl_easy_setopt(BinaCPP::curl, CURLOPT_POSTFIELDS, post_data.c_str() );
 		}

		res = curl_easy_perform(BinaCPP::curl);

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			LOG_INFO << "<BinaCPP::curl_api> curl_easy_perform() failed: " << curl_easy_strerror(res)  ;
		} 	

	}
}



