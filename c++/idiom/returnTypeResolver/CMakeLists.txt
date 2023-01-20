cmake_minimum_required( VERSION 3.20)

project(returnTypeResolver)

set(SOURCE ${CMAKE_SOURCE_DIR}/returnTypeResolver.cpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

add_executable(${PROJECT_NAME} ${SOURCE})
