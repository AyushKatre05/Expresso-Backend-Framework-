# Expresso - HTTP backend with optional parser
cmake_minimum_required(VERSION 3.13)
project(Expresso LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

# Expresso parser static library (C)
set(EXPRESSO_PARSER_SOURCES
  expresso-parser/request_parse.c
  expresso-parser/request_builder.c
  expresso-parser/memory.c
)
add_library(expresso_parser STATIC ${EXPRESSO_PARSER_SOURCES})
target_include_directories(expresso_parser PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/expresso-types
  ${CMAKE_CURRENT_SOURCE_DIR}/expresso-parser
)

# Expresso HTTP server (C++)
file(GLOB_RECURSE EXPRESSO_SERVER_SOURCES expresso-server/src/*.cpp expresso-server/src/*.hpp)
list(FILTER EXPRESSO_SERVER_SOURCES EXCLUDE REGEX ".*express/express_bridge\\.(cpp|hpp)$")

add_executable(expresso-server ${EXPRESSO_SERVER_SOURCES})
target_include_directories(expresso-server PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/expresso-server/src
  ${CMAKE_CURRENT_SOURCE_DIR}/expresso-types
  ${CMAKE_CURRENT_SOURCE_DIR}/expresso-parser
)
target_link_libraries(expresso-server PRIVATE Threads::Threads ZLIB::ZLIB)

# Option: build with Expresso parser connected
option(USE_EXPRESS_PARSER "Link Expresso server with parser" ON)
if(USE_EXPRESS_PARSER)
  target_sources(expresso-server PRIVATE expresso-server/src/express/express_bridge.cpp)
  target_compile_definitions(expresso-server PRIVATE USE_EXPRESS_PARSER)
  target_link_libraries(expresso-server PRIVATE expresso_parser)
  target_include_directories(expresso-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)
