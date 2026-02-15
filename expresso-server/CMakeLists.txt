cmake_minimum_required(VERSION 3.13)

project(expresso-server LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.hpp)
# Express bridge only used when building from repo root with USE_EXPRESS_PARSER
if(NOT USE_EXPRESS_PARSER)
    list(FILTER SOURCE_FILES EXCLUDE REGEX ".*express/express_bridge\\.(cpp|hpp)$")
endif()

set(CMAKE_CXX_STANDARD 23)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)
find_package(ZLIB REQUIRED)

add_executable(expresso-server ${SOURCE_FILES})
target_include_directories(expresso-server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(expresso-server PRIVATE Threads::Threads ZLIB::ZLIB)
