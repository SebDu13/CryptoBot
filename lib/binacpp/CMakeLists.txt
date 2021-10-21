cmake_minimum_required(VERSION 3.1)
project(CryptoBot2)

find_package(CURL REQUIRED) 

include_directories(
	include
	../jsoncpp-1.8.3/include
	${CURL_INCLUDE_DIR}
)

add_library (
	binacpp
	src/binacpp.cpp
	src/binacpp_logger.cpp
	src/binacpp_utils.cpp
	src/binacpp_websocket.cpp)