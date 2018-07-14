cmake_minimum_required(VERSION 3.3.2)
project(eftp-client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.c commands.h commands.c)
add_executable(eftp-client ${SOURCE_FILES})