cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)

project(external_sort CXX)

set(CMAKE_CXX_STANDARD 11)

set(SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_executable(sort ${SRC}/main.cpp)
