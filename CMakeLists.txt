cmake_minimum_required(VERSION 3.9)
project(ompe LANGUAGES CXX)

find_package(OpenMP REQUIRED)
add_executable(ompe main.cpp)
target_link_libraries(ompe PRIVATE OpenMP::OpenMP_CXX)