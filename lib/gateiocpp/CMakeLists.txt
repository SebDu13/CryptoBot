cmake_minimum_required(VERSION 3.1)
project(CryptoBot2)

find_package(CURL REQUIRED) 

include_directories(
	${CURL_INCLUDE_DIR}
	../jsoncpp-1.8.3/include
	include
)

set(BINANCE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/binacpp/)
set(BINANCE_INCLUDE_DIR ${BINANCE_LIB_DIR}/include)

include_directories(
  ${BINANCE_INCLUDE_DIR}
)

add_library (
	gateiocpp
	src/gateiocpp.cpp)