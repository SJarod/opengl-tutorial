cmake_minimum_required(VERSION 3.21)

set(CMAKE_CXX_STANDARD 20)

message(STATUS "updating submodules")
execute_process(COMMAND git submodule update --init --recursive
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    RESULT_VARIABLE GIT_RESULT
)
if(NOT GIT_RESULT EQUAL "0")
    message(FATAL_ERROR "Failed to update submodules, ${GIT_RESULT}")
endif()

project(opengl-tutorial)

add_subdirectory(externals)

add_executable(gl src/main.cpp)
target_include_directories(gl PRIVATE include)
target_link_libraries(gl
    PUBLIC glad
    PUBLIC glfw
)