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
#include "gateiocpp.h"
#include "logger.h"

#include "binacpp.h"

using namespace std;

string GateIoCPP::api_key = "";
string GateIoCPP::secret_key = "";
CURL* GateIoCPP::curl = NULL;




//---------------------------------
void 
GateIoCPP::init( string &api_key, string &secret_key ) 
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	GateIoCPP::curl = curl_easy_init();
	GateIoCPP::api_key = api_key;
	GateIoCPP::secret_key = secret_key;
}


void
GateIoCPP::cleanup()
{
	curl_easy_cleanup(GateIoCPP::curl);
	curl_global_cleanup();
}

//------------------
//GET api/v1/exchangeInfo
//------------------
void 
GateIoCPP::get_currency_pairs( Json::Value &json_result)
{
	LOG_INFO <<  "<GateIoCPP::currency_pairs>";

	string url(GATEIO_HOST);  
	url += "/api/v4/spot/currency_pairs";

	string str_result;
	curl_api( url, str_result ) ;

	if ( str_result.size() > 0 ) {
		
		try {
			Json::Reader reader;
			json_result.clear();	
			reader.parse( str_result , json_result );
	    		
		} catch ( exception &e ) {
		 	LOG_INFO <<  "<GateIoCPP::currency_pairs> Error ! " << e.what(); 
		}   
		LOG_INFO <<  "<GateIoCPP::currency_pairs> Done.";
	
	} else {
		LOG_INFO <<  "<GateIoCPP::currency_pairs> Failed to get anything.";
	}
}

//-----------------
// Curl's callback
size_t 
GateIoCPP::curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	LOG_INFO <<  "<GateIoCPP::curl_cb> ";

	buffer->append((char*)content, size*nmemb);

	LOG_INFO <<  "<GateIoCPP::curl_cb> done";
	return size*nmemb;
}






//--------------------------------------------------
void 
GateIoCPP::curl_api( string &url, string &result_json ) {
	vector <string> v;
	string action = "GET";
	string post_data = "";
	curl_api_with_header( url , result_json , v, post_data , action );	
} 



//--------------------
// Do the curl
void 
GateIoCPP::curl_api_with_header( string &url, string &str_result, vector <string> &extra_http_header , string &post_data , string &action ) 
{

	LOG_INFO <<  "<GateIoCPP::curl_api>";

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
			LOG_INFO <<  "<GateIoCPP::curl_api> curl_easy_perform() failed: " << curl_easy_strerror(res);
		} 	

	}

	LOG_INFO <<  "<GateIoCPP::curl_api> done";

}



