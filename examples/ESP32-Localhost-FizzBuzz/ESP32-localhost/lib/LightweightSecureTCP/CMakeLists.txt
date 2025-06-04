# lib/CMakeLists.txt
cmake_minimum_required(VERSION 3.16)

project(LightweightSecureTCP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect sources
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

add_library(LightweightSecureTCP STATIC ${LIB_SOURCES})

target_include_directories(LightweightSecureTCP PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(LightweightSecureTCP PRIVATE LIGHTWEIGHT_SECURE_TCP_LIBRARY)
