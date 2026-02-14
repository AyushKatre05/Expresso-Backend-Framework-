# Top-level: build HTTP server with optional Express parser (loosely coupled)
cmake_minimum_required(VERSION 3.13)
project(ExpressoBackend LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

# Express parser static library (C)
set(EXPRESS_PARSER_SOURCES
  ExpressParser/request_parse.c
  ExpressParser/request_builder.c
  ExpressParser/memory.c
)
add_library(express_parser STATIC ${EXPRESS_PARSER_SOURCES})
target_include_directories(express_parser PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/ExpressTypes
  ${CMAKE_CURRENT_SOURCE_DIR}/ExpressParser
)

# HTTP server (C++) - sources from subfolders: config, constants, io, utils, core
file(GLOB_RECURSE HTTP_SOURCES HTTP/src/*.cpp HTTP/src/*.hpp)
list(FILTER HTTP_SOURCES EXCLUDE REGEX ".*express/express_bridge\\.(cpp|hpp)$")

add_executable(http-server ${HTTP_SOURCES})
target_include_directories(http-server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/HTTP/src
  ${CMAKE_CURRENT_SOURCE_DIR}/ExpressTypes
  ${CMAKE_CURRENT_SOURCE_DIR}/ExpressParser
)
target_link_libraries(http-server PRIVATE Threads::Threads ZLIB::ZLIB)

# Option: build with Express parser connected
option(USE_EXPRESS_PARSER "Link HTTP server with Express parser" ON)
if(USE_EXPRESS_PARSER)
  target_sources(http-server PRIVATE HTTP/src/express/express_bridge.cpp)
  target_compile_definitions(http-server PRIVATE USE_EXPRESS_PARSER)
  target_link_libraries(http-server PRIVATE express_parser)
  target_include_directories(http-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
