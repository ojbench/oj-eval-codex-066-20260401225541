cmake_minimum_required(VERSION 3.10)
project(any_ptr_066)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(code main.cpp)
target_sources(code PRIVATE src.hpp)

