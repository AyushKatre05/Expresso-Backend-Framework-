cmake_minimum_required(VERSION 3.13)

project(expresso-backend LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 11)

# Options
option(USE_EXPRESS_PARSER "Use the custom C parser" OFF)

# Dependencies
find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

# Include directories
include_directories(expresso-types)

# Build Parser Library (Static)
add_library(expresso-parser STATIC
    expresso-parser/request_parse.c
    expresso-parser/request_builder.c
    expresso-parser/memory.c
    # parser.c is just a wrapper, we use the split files
)
target_include_directories(expresso-parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Build Server
add_subdirectory(expresso-server)

# Link Parser if requested
if(USE_EXPRESS_PARSER)
    target_link_libraries(expresso-server PRIVATE expresso-parser)
    target_compile_definitions(expresso-server PRIVATE USE_EXPRESS_PARSER)
endif()
