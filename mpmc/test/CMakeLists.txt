cmake_minimum_required(VERSION 3.14)

set(This MpmcTest)

set(Sources 
    mpmc_tests.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} PUBLIC
    gtest_main
    Mpmc
)

gtest_discover_tests(${This})