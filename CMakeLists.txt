cmake_minimum_required(VERSION 3.16)
 
project(CryptoBot2)

set(CMAKE_BUILD_TYPE release)

message("Compiler Version: ${CMAKE_CXX_COMPILER}")

find_package(OpenSSL REQUIRED)

#set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost 1.71.0 COMPONENTS log program_options REQUIRED)

add_definitions(-std=c++2a -DBOOST_LOG_DYN_LINK)

set(BINANCE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/binacpp/)
set(BINANCE_INCLUDE_DIR ${BINANCE_LIB_DIR}/include)
set(GATEIO_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/gateiocpp/)
set(GATEIO_INCLUDE_DIR ${GATEIO_LIB_DIR}/include)
set(KUCOIN_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/kucoincpp/)
set(KUCOIN_INCLUDE_DIR ${KUCOIN_LIB_DIR}/include)
set(JSONCPP_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/jsoncpp-1.8.3/)
set(JSONCPP_INCLUDE_DIR ${JSONCPP_LIB_DIR}/include)
set(TOOLS_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/tools/)
set(TOOLS_INCLUDE_DIR ${TOOLS_LIB_DIR}/include)

file(GLOB TOOLS_SOURCES CONFIGURE_DEPENDS ${TOOLS_LIB_DIR}/src/*.cpp)
file(GLOB TOOLS_HEADERS CONFIGURE_DEPENDS ${TOOLS_LIB_DIR}/include/*.h)
add_library(
	tools
	${TOOLS_HEADERS}
	${TOOLS_SOURCES}
)

file(GLOB JSON_SOURCES CONFIGURE_DEPENDS ${JSONCPP_LIB_DIR}/src/*.cpp)
file(GLOB JSON_HEADERS CONFIGURE_DEPENDS ${JSONCPP_LIB_DIR}/include/*.h)
add_library(
	jsoncpp
	${JSON_HEADERS}
	${JSON_SOURCES}
)

file(GLOB BINANCE_SOURCES CONFIGURE_DEPENDS ${BINANCE_LIB_DIR}/src/*.cpp)
file(GLOB BINANCE_HEADERS CONFIGURE_DEPENDS ${BINANCE_LIB_DIR}/include/*.h)
add_library(
	binance
	${BINANCE_HEADERS}
	${BINANCE_SOURCES}
)

file(GLOB GATEIO_SOURCES CONFIGURE_DEPENDS ${GATEIO_LIB_DIR}/src/*.cpp)
file(GLOB GATEIO_HEADERS CONFIGURE_DEPENDS ${GATEIO_LIB_DIR}/include/*.h)
add_library(
	gateio
	${GATEIO_HEADERS}
	${GATEIO_SOURCES}
)

file(GLOB KUCOIN_SOURCES CONFIGURE_DEPENDS ${KUCOIN_LIB_DIR}/src/*.cpp)
file(GLOB KUCOIN_HEADERS CONFIGURE_DEPENDS ${KUCOIN_LIB_DIR}/include/*.h)
add_library(
	kucoin
	${KUCOIN_HEADERS}
	${KUCOIN_SOURCES}
)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${TOOLS_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIR}
  ${BINANCE_INCLUDE_DIR}
  ${GATEIO_INCLUDE_DIR}
  ${KUCOIN_INCLUDE_DIR}
  ${Boost_INCLUDE_DIR}
)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(
	${PROJECT_NAME}
	${PROJECT_SOURCES}
)

target_link_libraries (
	${PROJECT_NAME}
	${Boost_LIBRARIES}
	jsoncpp
	binance
	gateio
	kucoin
	tools
	curl
	OpenSSL::Crypto
)

