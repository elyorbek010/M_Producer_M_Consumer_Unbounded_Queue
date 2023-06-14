cmake_minimum_required(VERSION 3.14)

set(This MpmcBenchmark)

set(Sources 
    mpmc_benchmark.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    Mpmc
    benchmark::benchmark
)