cmake_minimum_required(VERSION 3.1)
project(CryptoBot2)

include_directories(
	include
)

add_library (
	jsoncpp
	src/jsoncpp.cpp)