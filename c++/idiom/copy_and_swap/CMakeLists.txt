cmake_minimum_required( VERSION 3.20)

project(swap_copy)

set(SOURCE ${CMAKE_SOURCE_DIR}/swap_copy.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_executable(${PROJECT_NAME} ${SOURCE})