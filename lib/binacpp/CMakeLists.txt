cmake_minimum_required(VERSION 3.1)
project(binacpp)

find_package(CURL REQUIRED) 
find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include_directories(${CURL_INCLUDE_DIR})
include_directories("${JSONCPP_INCLUDE_DIRS}")
include_directories(include)

add_library (
	binacpp
	src/binacpp.cpp
	src/binacpp_logger.cpp
	src/binacpp_utils.cpp
	src/binacpp_websocket.cpp)