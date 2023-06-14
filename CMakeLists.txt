cmake_minimum_required(VERSION 3.14)

project(BenchProject C CXX)

set(BENCHMARK_ENABLE_TESTING OFF)

enable_testing()

#add_subdirectory(googletest)

include(FetchContent)
FetchContent_Declare(
  googletest
  # Specify the commit you depend on and update it regularly.
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

#add_subdirectory(benchmark)

include(FetchContent)
FetchContent_Declare(
  benchmark
  URL https://github.com/google/benchmark/archive/refs/tags/v1.8.0.zip
)
FetchContent_MakeAvailable(benchmark)


add_subdirectory(mpmc)