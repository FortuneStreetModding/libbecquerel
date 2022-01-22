cmake_minimum_required(VERSION 3.10)

project(libbecquerel)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(becquerel brlyt.cpp common.cpp)
