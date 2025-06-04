cmake_minimum_required(VERSION 3.16)
project(DesktopServerExample LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the LightweightSecureTCP library
add_subdirectory(lib/LightweightSecureTCP)

add_executable(desktop_server main.cpp)
target_link_libraries(desktop_server PRIVATE LightweightSecureTCP)
target_include_directories(desktop_server PRIVATE
    lib/LightweightSecureTCP/include
)
