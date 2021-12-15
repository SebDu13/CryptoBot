#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <exception>
#include <chrono>

#include "HuobiApi.hpp"
#include "logger.hpp"
#include "magic_enum.hpp"
#include "sha.hpp"
#include "chrono.hpp"
#include "tools.hpp"
#include "base64.h"
#include "utf8.h"

#include "cct_codec.hpp"
#include "cct_nonce.hpp"
#include "ssl_sha.hpp"

namespace{

std::string query(CURL *curl, std::string_view url, const cct::CurlOptions &opts) {

  // General option settings.
  const char *optsStr = opts.postdata.c_str();

  std::string modifiedURL(url);
  std::string jsonBuf;  // Declared here as its scope should be valid until the actual curl call
  if (opts.requestType() != cct::CurlOptions::RequestType::kPost && !opts.postdata.empty()) {
    // Add parameters as query std::string after the URL
    modifiedURL.push_back('?');
    modifiedURL.append(opts.postdata.str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
  } else {
    if (opts.postdataInJsonFormat && !opts.postdata.empty()) {
      jsonBuf = opts.postdata.toJson().asString();
      optsStr = jsonBuf.c_str();
    }
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, optsStr);
  }

  LOG_INFO << opts.requestTypeStr() << url << (opts.postdata.empty() ? "" : " opts ") << optsStr;

  curl_easy_setopt(curl, CURLOPT_URL, modifiedURL.c_str());
  curl_easy_setopt(curl, CURLOPT_USERAGENT, opts.userAgent);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, opts.followLocation);

  // Important! We should reset ALL fields of curl object at each call to query,
  // as it would be possible for a new query to read from a dangling reference form a previous
  // query.
  curl_easy_setopt(curl, CURLOPT_POST, opts.requestType() == cct::CurlOptions::RequestType::kPost);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,
                   opts.requestType() == cct::CurlOptions::RequestType::kDelete ? "DELETE" : nullptr);
  if (opts.requestType() == cct::CurlOptions::RequestType::kGet) {
    // This is to force cURL to switch in a GET request
    // Useless to reset to 0 in other cases
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
  }

  curl_easy_setopt(curl, CURLOPT_VERBOSE, opts.verbose ? 1L : 0L);
  curl_slist *curlListPtr = nullptr, *oldCurlListPtr = nullptr;
  for (const std::string &header : opts.httpHeaders) {
    curlListPtr = curl_slist_append(curlListPtr, header.c_str());
    if (!curlListPtr) {
      if (oldCurlListPtr) {
        curl_slist_free_all(oldCurlListPtr);
      }
      throw std::bad_alloc();
    }
    oldCurlListPtr = curlListPtr;
  }
  using CurlListUniquePtr =
      std::unique_ptr<curl_slist, decltype([](curl_slist *hdrList) { curl_slist_free_all(hdrList); })>;
  CurlListUniquePtr curlListUniquePtr(curlListPtr);

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlListPtr);
  std::string out;
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);

  //setUpProxy(opts.proxy);

  const CURLcode res = curl_easy_perform(curl);  // Get reply

  if (res != CURLE_OK) 
  {
	  LOG_INFO << out;
	throw std::runtime_error("Unexpected response from curl: Error " + std::to_string(res));
  }
  return out;
}

std::string urlEncode(CURL *curl, std::string_view url) {

  using CurlStringUniquePtr = std::unique_ptr<char, decltype([](char *ptr) { curl_free(ptr); })>;

  CurlStringUniquePtr uniquePtr(curl_easy_escape(curl, url.data(), static_cast<int>(url.size())));
  const char *encodedChars = uniquePtr.get();
  if (!encodedChars) {
    throw std::bad_alloc();
  }
  return encodedChars;
}

void fix_utf8_string(std::string& str)
{
    std::string temp;
    utf8::replace_invalid(str.begin(), str.end(), back_inserter(temp));
    str = temp;
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

size_t curl_cb( void *content, size_t size, size_t nmemb, std::string *buffer ) 
{	
	buffer->append((char*)content, size*nmemb);
	return size*nmemb;
}

}

HuobiApi::HuobiApi(const std::string &api_key, const std::string &secret_key, const std::string &passPhrase )
: curl(curl_easy_init())
, api_key(api_key)
, secret_key(secret_key)
, passPhrase(passPhrase)
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
}

HuobiApi::~HuobiApi()
{
	LOG_INFO;
	cleanup();
}

void HuobiApi::cleanup()
{
	LOG_INFO;
	curl_easy_cleanup(HuobiApi::curl);
	curl_global_cleanup();
}

/* TEST WITH MEXC EXCHANGE */
void HuobiApi::getMexcTicker(const std::string& pairId, SpotTickersResult &json_result) const
{
	CHRONO_THIS_SCOPE;
    getMexcTickersGeneric("/open/api/v2/market/ticker?symbol=" + pairId, json_result);
}

void HuobiApi::getMexcTickersGeneric(const std::string& url, SpotTickersResult &json_result) const
{
	std::string _url("https://www.mexc.com");  
	_url += url;

	std::string str_result;
	curl_api( _url, str_result );

	if ( !str_result.empty() ) 
	{	
		convertToJson(str_result, json_result);
	}
	else
		LOG_ERROR <<  "Failed to get anything.";
}
/* END TEST */  

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
void HuobiApi::getTicker(const std::string& pairId, SpotTickersResult &json_result) const
{
    getTickersGeneric("/market/detail/merged?symbol=" + pairId, json_result);
}

void HuobiApi::getTickersGeneric(const std::string& url, SpotTickersResult &json_result) const
{
	std::string _url(kUrlBase);  
	_url += url;

	std::string str_result;
	curl_api( _url, str_result );

	if ( !str_result.empty() ) 
	{	
		convertToJson(str_result, json_result);
	}
	else
		LOG_ERROR <<  "Failed to get anything.";
}

/*{
        "code" : "400100",
        "msg" : "Order size below the minimum requirement."
}
{
    "code" : "900001",
    "msg" : "Symbol [pouette-USDT] Not Exists"
}*/
void HuobiApi::sendLimitOrder ( 
	const std::string& currency_pair, 
	const Side side,
	const TimeInForce timeInForce,
	const Quantity& quantity,
	const Price& price,
	Json::Value &json_result ) const
{
	//LOG_DEBUG;
	//CHRONO_THIS_SCOPE;

	if ( api_key.size() == 0 || secret_key.size() == 0 )
	{
		LOG_ERROR << "API Key and Secret Key has not been set.";
		return ;
	}

	std::string url(kUrlBase);
	std::string prefix("/api/v1/orders");
	url += prefix;

	Json::Value bodyJson;
	bodyJson["clientOid"] = std::to_string(duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count());
	bodyJson["side"] = std::string(magic_enum::enum_name(side));
	bodyJson["symbol"] = currency_pair;
	bodyJson["type"] = "limit";
	bodyJson["stp"] = "CO";
	bodyJson["price"] = price.toString();
	bodyJson["size"] = quantity.toString();
	bodyJson["timeInForce"] = std::string(magic_enum::enum_name(timeInForce));

	std::string body = bodyJson.toStyledString();

	//LOG_DEBUG << "url = " << url << " body = " << body;
	
	std::string action("POST");
	const auto httpHeader = generateSignedHttpHeader(action, prefix, body);

	LOG_DEBUG;
	std::string result;
	curl_api_with_header( url, httpHeader, body, action, result ) ;
	if ( result.size() > 0 ) 
	{
		convertToJson(result, json_result); 
	} 
	else 
		LOG_ERROR << "Failed to get anything.";
}

void HuobiApi::getOrder ( 
	const std::string& orderId,
	Json::Value &json_result ) const
{
	CHRONO_THIS_SCOPE;

	if ( api_key.size() == 0 || secret_key.size() == 0 )
	{
		LOG_ERROR << "API Key and Secret Key has not been set.";
		return ;
	}

	std::string url(kUrlBase);
	std::string prefix("/api/v1/orders/" + orderId);
	url += prefix;
	
	//LOG_DEBUG << url;

	std::string action("GET");
	std::string body;
	const auto httpHeader = generateSignedHttpHeader(action, prefix, body);

	std::string result;
	curl_api_with_header( url, httpHeader, body, action, result ) ;
	if ( result.size() > 0 ) 
	{
		convertToJson(result, json_result); 
	} 
	else 
		LOG_ERROR << "Failed to get anything.";
}

void HuobiApi::getAccounts(Json::Value &json_result) const
{
	json_result = PrivateQuery(cct::CurlOptions::RequestType::kGet, "/v1/account/accounts");
}

void HuobiApi::curl_api( std::string &url, std::string &result_json ) const
{
	curl_api_with_header( url , {}, "" , "GET", result_json );	
}

Json::Value HuobiApi::PrivateQuery(cct::CurlOptions::RequestType requestType, std::string_view method, const cct::CurlPostData& postdata) const
{
  std::string url = kUrlBase;
  url.append(method);

  cct::Nonce nonce = cct::Nonce_LiteralDate();
  std::string encodedNonce = urlEncode(curl, nonce);

  cct::CurlOptions opts(requestType);
  opts.userAgent = kUserAgent;

  opts.httpHeaders.push_back("Content-Type: application/json");
  // Remove 'https://' (which is 8 chars) from URL base
  std::string paramsStr(opts.requestTypeStr());
  paramsStr.push_back('\n');
  paramsStr.append(kUrlBase + 8);
  paramsStr.push_back('\n');
  paramsStr.append(method);
  paramsStr.push_back('\n');

  cct::CurlPostData signaturePostdata;

  signaturePostdata.append("AccessKeyId", api_key);
  signaturePostdata.append("SignatureMethod", "HmacSHA256");
  signaturePostdata.append("SignatureVersion", "2");
  signaturePostdata.append("Timestamp", encodedNonce);
  if (!postdata.empty()) {
    if (requestType == cct::CurlOptions::RequestType::kGet) {
      signaturePostdata.append(postdata);
    } else {
      opts.postdataInJsonFormat = true;
      opts.postdata = postdata;
    }
  }

  paramsStr.append(signaturePostdata.str());

  std::string sig = urlEncode(curl, cct::B64Encode(cct::ssl::ShaBin(cct::ssl::ShaType::kSha256, paramsStr, secret_key)));
  //std::string sig = B64Encode(ssl::ShaBin(ssl::ShaType::kSha256, paramsStr, apiKey.privateKey()));

  signaturePostdata.append("Signature", sig);
  url.push_back('?');
  url.append(signaturePostdata.str());

  Json::Value ret;
  convertToJson(query(curl, url, opts), ret);

  /*if (ret.contains("status") && ret["status"].get<std::string_view>() != "ok") {
    std::string errMsg("Error: ");
    if (ret.contains("err-msg")) {
      errMsg.append(ret["err-msg"].get<std::string_view>());
    }
    throw std::runtime_error(std::move(errMsg));
  }*/

  return ret;
}


std::vector <std::string> HuobiApi::generateSignedHttpHeader(const std::string& action, const std::string& prefix, const std::string& body) const
{
	using namespace cct;

	auto timeStamp = tools::get_current_ms_epoch();
	std::stringstream sign;
	sign << timeStamp << action << prefix << body;

	std::string signHash = B64Encode(ssl::ShaBin(ssl::ShaType::kSha256, sign.str(), secret_key));
	std::string passPhraseHash = B64Encode(ssl::ShaBin(ssl::ShaType::kSha256, passPhrase, secret_key));

	Nonce nonce = Nonce_LiteralDate();
	std::string encodedNonce = urlEncode(curl, nonce);

	return std::vector <std::string>{
		 "AccessKeyId:" + secret_key
		,"SignatureMethod:HmacSHA256" 
		,"SignatureVersion:2"
		,"Timestamp:" + encodedNonce};


}

void HuobiApi::curl_api_with_header(const std::string &url
			,const std::vector <std::string> &extra_http_header
			,const std::string &post_data
			,const std::string &action
			, std::string &str_result) const
{
	//CHRONO_THIS_SCOPE;
	CURLcode res;

	if( curl ) 
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &str_result );
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_ENCODING, "gzip");
		//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		if ( extra_http_header.size() > 0 ) {
			
			struct curl_slist *chunk = NULL;
			for ( int i = 0 ; i < extra_http_header.size() ;i++ ) 
			{
				chunk = curl_slist_append(chunk, extra_http_header[i].c_str() );
			}
			curl_easy_setopt(HuobiApi::curl, CURLOPT_HTTPHEADER, chunk);
		}

		if ( action == "PUT" || action == "DELETE" ) 
			curl_easy_setopt(HuobiApi::curl, CURLOPT_CUSTOMREQUEST, action.c_str() );

		if(!post_data.empty() || action == "POST")
			curl_easy_setopt(HuobiApi::curl, CURLOPT_POSTFIELDS, post_data.c_str() );

		res = curl_easy_perform(HuobiApi::curl);
		curl_easy_reset(curl); // reset the options

		/* Check for errors */ 
		if ( res != CURLE_OK ) {
			LOG_ERROR <<  "failed: " << curl_easy_strerror(res);
		}
	}
	else 
		LOG_ERROR <<  "curl is null";
}



