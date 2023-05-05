cmake_minimum_required(VERSION 3.16)

project(circularque)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

file(GLOB  SOURCE
    "${CMAKE_SOURCE_DIR}/*.h"
    "${CMAKE_SOURCE_DIR}/*.cc"
    )  


add_executable(${PROJECT_NAME} ${SOURCE})
