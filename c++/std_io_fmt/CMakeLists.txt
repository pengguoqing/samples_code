cmake_minimum_required(VERSION 3.0)
project(statistic_scores)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_executable(statistic_scores ${CMAKE_SOURCE_DIR}/source.cc)

