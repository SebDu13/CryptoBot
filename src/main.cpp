#include <map>
#include <vector>
#include <string>
#include <json/json.h>


#include "binacpp.h"
#include "gateiocpp.h"


int main() 
{
	std::string api_key	= "api key";
	std::string secret_key 	= "user key";
	BinaCPP::init( api_key , secret_key );
	GateIoCPP::init( api_key , secret_key );

	Json::Value result;
	//BinaCPP::get_exchangeInfo(result);
	//BinaCPP::get_exchangeInfo(result);
	//GateIoCPP::get_currency_pairs(result);
	GateIoCPP::get_currency_pairs(result);

	cout << result[1]["id"] << endl;

	/*for(const auto& val : result["symbols"])
	{
 		cout << val["symbol"] << endl;
 		cout << val["quoteAsset"] << endl;
 		break;
	}*/

	return 0;	
}