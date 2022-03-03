cmake_minimum_required(VERSION 3.10)

project(libbecquerel VERSION 0.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(becquerel brlan.cpp brlyt.cpp common.cpp)

add_executable(lyttest lyttest.cpp)
target_link_libraries(lyttest PUBLIC becquerel)
add_executable(lantest lantest.cpp)
target_link_libraries(lantest PUBLIC becquerel)
