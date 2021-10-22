cmake_minimum_required(VERSION 3.16)
 
project(CryptoBot2)

set(CMAKE_BUILD_TYPE Debug)

find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED COMPONENTS log log_setup)

add_definitions(-std=c++17 -DBOOST_LOG_DYN_LINK)

set(BINANCE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/binacpp/)
set(BINANCE_INCLUDE_DIR ${BINANCE_LIB_DIR}/include)
set(GATEIO_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/gateiocpp/)
set(GATEIO_INCLUDE_DIR ${GATEIO_LIB_DIR}/include)
set(JSONCPP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp-1.8.3/)
set(JSONCPP_INCLUDE_DIR ${JSONCPP_LIB_DIR}/include)
set(TOOLS_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/tools/)
set(TOOLS_INCLUDE_DIR ${TOOLS_LIB_DIR}/include)

file(GLOB TOOLS_SOURCES ${TOOLS_LIB_DIR}/src/*.cpp)
file(GLOB TOOLS_HEADERS ${TOOLS_LIB_DIR}/include/*.h)
add_library(
	tools
	${TOOLS_HEADERS}
	${TOOLS_SOURCES}
)

file(GLOB JSON_SOURCES ${JSONCPP_LIB_DIR}/src/*.cpp)
file(GLOB JSON_HEADERS ${JSONCPP_LIB_DIR}/include/*.h)
add_library(
	jsoncpp
	${JSON_HEADERS}
	${JSON_SOURCES}
)

file(GLOB BINANCE_SOURCES ${BINANCE_LIB_DIR}/src/*.cpp)
file(GLOB BINANCE_HEADERS ${BINANCE_LIB_DIR}/include/*.h)
add_library(
	binance
	${BINANCE_HEADERS}
	${BINANCE_SOURCES}
)

file(GLOB GATEIO_SOURCES ${GATEIO_LIB_DIR}/src/*.cpp)
file(GLOB GATEIO_HEADERS ${GATEIO_LIB_DIR}/include/*.h)
add_library(
	gateio
	${GATEIO_HEADERS}
	${GATEIO_SOURCES}
)

include_directories(
  ${TOOLS_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIR}
  ${BINANCE_INCLUDE_DIR}
  ${GATEIO_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
)

target_link_libraries (
	${PROJECT_NAME}
	${Boost_LIBRARIES}
	jsoncpp
	binance
	gateio
	tools
	curl
	OpenSSL::Crypto
)

