cmake_minimum_required(VERSION 3.14)

set(This Mpmc)

project(${This} C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENTEND_CODE ON)

enable_testing()

set(Headers 
    vector.h
    debug.h
)

set(Sources
    vector.c
)

add_library(${This} STATIC ${Sources} ${Headers})

add_subdirectory(test)
add_subdirectory(benchmark)