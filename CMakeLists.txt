cmake_minimum_required(VERSION 3.10)

project(libbecquerel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(becquerel brlyt.cpp common.cpp)

add_executable(bqtest test.cpp)
target_link_libraries(bqtest PUBLIC becquerel)
