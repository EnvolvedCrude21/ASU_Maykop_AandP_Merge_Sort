cmake_minimum_required(VERSION 4.0)
project(merge_sort)

set(CMAKE_CXX_STANDARD 26)

add_executable(merge_sort
        sort.cpp
        merge_sort_visualizer.cpp
)
