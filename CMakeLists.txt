cmake_minimum_required(VERSION 3.16)
 
project(CryptoBot2)

set(CMAKE_BUILD_TYPE Debug)

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
link_libraries(${JSONCPP_LIBRARIES})

#find_package(CURL REQUIRED)

find_package(OpenSSL REQUIRED)

set(BINANCE_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/binacpp/)
set(BINANCE_INCLUDE_DIR ${BINANCE_LIB_DIR}/include)
set(GATEIO_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/gateiocpp/)
set(GATEIO_INCLUDE_DIR ${GATEIO_LIB_DIR}/include)

include_directories(
  ${BINANCE_INCLUDE_DIR}
  ${JSONCPP_INCLUDE_DIRS}
  ${GATEIO_INCLUDE_DIR}
)

add_executable(
	${PROJECT_NAME}
	src/main.cpp
)

target_link_libraries (
	${PROJECT_NAME}
	${BINANCE_LIB_DIR}/libbinacpp.a
	${GATEIO_LIB_DIR}/libgateiocpp.a
	curl
	OpenSSL::Crypto
	websockets
)