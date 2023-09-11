cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(opengl_sample)

set(CMAKE_CXX_STANDARD 17) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${CMAKE_SOURCE_DIR}/lib)

set(LIBS glfw3 yuv)

set(demos
    fbo_pbo
    wgl_context
    
   )

#cur version need copy yuv lib and dll to related directory
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/common/libyuv)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_library(GLAD STATIC)
target_sources(GLAD PRIVATE ${CMAKE_SOURCE_DIR}/src/common/gl.c ${CMAKE_SOURCE_DIR}/include/glad/gl.h)
target_sources(GLAD PRIVATE  ${CMAKE_SOURCE_DIR}/src/common/wgl.c ${CMAKE_SOURCE_DIR}/include/glad/wgl.h)


set(LIBS ${LIBS} GLAD)  
add_library(STBIMG ${CMAKE_SOURCE_DIR}/src/common/stb_image.cpp)
set(LIBS ${LIBS} STBIMG)
  

message(${LIBS})

function(CreateProjectFromSubDir demo)
    message(${demo})

    file(GLOB SOURCE
        "${CMAKE_SOURCE_DIR}/src/${demo}/*.h"
        "${CMAKE_SOURCE_DIR}/src/${demo}/*.cpp"
        "${CMAKE_SOURCE_DIR}/src/${demo}/*.cc"
        "${CMAKE_SOURCE_DIR}/src/${demo}/*.vs"
        "${CMAKE_SOURCE_DIR}/src/${demo}/*.fs"
        )    
   
    set(NAME "${demo}")
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    target_link_options(${NAME}   PUBLIC  /ignore:4099)
    target_include_directories(${NAME} PRIVATE "${CMAKE_SOURCE_DIR}/src/${demo}")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${NAME}")
    set_target_properties(${NAME} PROPERTIES CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/lib")
    set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${NAME}/Debug")
    
    #copy shader to runtime dir
    file(GLOB SHADERS
        "${CMAKE_SOURE_DIR}/src/${demo}/*.vs"
        "${CMAKE_SOURE_DIR}/src/${demo}/*.fs"
        )
    #foreach(SHADER ${SHADERS})
    #   add_custom_command(TARGET ${NAME} PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} $<TARGET_FILE_DIR:${NAME}>)
    #endforeach(SHADER)
    
endfunction()

foreach(demo ${demos})
    CreateProjectFromSubDir(${demo})
endforeach(demo)


add_subdirectory(${CMAKE_SOURCE_DIR}/src/tutorials)