cmake_minimum_required(VERSION 3.16)

project(Homework1Benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(benchmark_core STATIC
    src/BenchmarkSuite.cpp
    src/Timer.cpp
)

target_include_directories(benchmark_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(Benchmark
    src/main.cpp
)

target_link_libraries(Benchmark PRIVATE benchmark_core)

add_executable(BenchmarkUTRunner
    tests/testMain.cpp
)

target_link_libraries(BenchmarkUTRunner PRIVATE benchmark_core)

enable_testing()
add_test(NAME BenchmarkUTRunner COMMAND BenchmarkUTRunner)
