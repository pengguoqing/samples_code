cmake_minimum_required (VERSION 3.0)
cmake_policy(VERSION 3.0)

project(GLTutorials)

set(tutorials
    01buffer_shader_glsl
    
   )

function(CreateProjectFromSubDir tutorial)
    message(${tutorial})

    file(GLOB SOURCE
        "${CMAKE_CURRENT_SOURCE_DIR}/${tutorial}/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/${tutorial}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/${tutorial}/*.cc"
        "${CMAKE_CURRENT_SOURCE_DIR}/${tutorial}/*.vs"
        "${CMAKE_CURRENT_SOURCE_DIR}/${tutorial}/*.fs"
        )    
   
    set(NAME "${tutorial}")
    add_executable(${NAME} ${SOURCE})
    target_link_libraries(${NAME} ${LIBS})
    target_link_libraries(${NAME} Opengl32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    target_link_options(${NAME}   PUBLIC  /ignore:4099)
    target_include_directories(${NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/${tutorial}")
    set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${NAME}")
    set_target_properties(${NAME} PROPERTIES CMAKE_ARCHIVE_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/lib")
    set_target_properties(${NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${NAME}/Debug")
    
    #copy shader to runtime dir
    file(GLOB SHADERS
        "${CMAKE_CURRENT_SOURCE_DIR}/${tutorial}/*.vs"
        "${CMAKE_CURRENT_SOURCE_DIR}/${tutorial}/*.ps"
        )

    foreach(SHADER ${SHADERS})
    add_custom_command(
        TARGET ${NAME} PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${SHADER}
                $<TARGET_FILE_DIR:${NAME}>)
    endforeach(SHADER)
    
endfunction()

foreach(tutorial ${tutorials})
    CreateProjectFromSubDir(${tutorial})
endforeach(tutorial)
