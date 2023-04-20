cmake_minimum_required(VERSION 3.16...3.25)

project(ffrw)


set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

file(GLOB SOURCE
"${CMAKE_SOURCE_DIR}/src/${demo}/*.h"
"${CMAKE_SOURCE_DIR}/src/${demo}/*.cc"
    )  

add_library(ffrw SHARED ${SOURCE})

link_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} avformat avcodec avutil swscale swresample)
target_compile_definitions(ffrw PRIVATE UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS _CRT_NONSTDC_NO_WARNINGS)

