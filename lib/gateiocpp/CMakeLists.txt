cmake_minimum_required(VERSION 3.1)
project(gateiocpp)

find_package(CURL REQUIRED) 
find_package(PkgConfig REQUIRED)

pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

include_directories(
	${CURL_INCLUDE_DIR}
	${JSONCPP_INCLUDE_DIRS}
	include
)

set(BINANCE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/binacpp/)
set(BINANCE_INCLUDE_DIR ${BINANCE_LIB_DIR}/include)

include_directories(
  ${BINANCE_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIRS}
)

add_library (
	gateiocpp
	src/gateiocpp.cpp)