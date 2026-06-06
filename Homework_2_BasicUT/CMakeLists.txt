cmake_minimum_required(VERSION 3.16)

project(Homework2BasicUT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(week_core STATIC
    src/Date.cpp
    src/WeekNumberCalculator.cpp
)

target_include_directories(week_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(BasicUT
    src/main.cpp
)

target_link_libraries(BasicUT PRIVATE week_core)

add_executable(BasicUTRunner
    tests/testMain.cpp
)

target_link_libraries(BasicUTRunner PRIVATE week_core)

enable_testing()
add_test(NAME BasicUTRunner COMMAND BasicUTRunner)
