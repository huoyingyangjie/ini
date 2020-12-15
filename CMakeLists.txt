cmake_minimum_required(VERSION 3.10)
project(ini CXX)

set(project_major "1")
set(project_minor "?")
set(project_hash  "?")
set(project_branch "?")
set(project_type "?")
set(project_time "?")

find_package(Git)

if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-list --all --count
            WORKING_DIRECTORY "./"
            OUTPUT_VARIABLE project_minor
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(project_minor STREQUAL "")
        set(project_minor  "?")
    endif()
endif()

if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            WORKING_DIRECTORY "./"
            OUTPUT_VARIABLE project_hash
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(project_hash STREQUAL "")
        set(project_hash  "?")
    endif()
endif()

if(GIT_FOUND)
    execute_process(
            COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
            WORKING_DIRECTORY "./"
            OUTPUT_VARIABLE project_branch
            ERROR_QUIET
            OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(project_branch STREQUAL "")
        set(project_branch  "?")
    endif()
endif()

list(APPEND build_options "Debug")
list(APPEND build_options "Release")
if(CMAKE_BUILD_TYPE IN_LIST build_options)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(project_type "D")
    else()
        set(project_type "R")
    endif()
else()
    set(project_type "D")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

string(TIMESTAMP project_time "%Y-%m-%d_%H:%M:%S")

set(VERSION "${project_major}.${project_minor}.${project_hash}.${project_branch}.${project_type}.${project_time}")
message(STATUS  "${VERSION}")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_ORG ${CMAKE_C_FLAGS})
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_ORG} -DVERSION='\"${VERSION}\"'  -fPIC -O3 -DNDEBUG -rdynamic -Wall -g")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_C_FLAGS_ORG} -DVERSION='\"${VERSION}\"'  -fPIC -O0 -rdynamic -Wall -g")

add_subdirectory(module/utils)

include_directories(include)
include_directories(module/utils/include)
aux_source_directory(src/ini INI_SRC)
add_library(ini STATIC ${INI_SRC})
add_library(ini_share SHARED ${INI_SRC})
set_target_properties(ini_share PROPERTIES OUTPUT_NAME ini)
target_link_libraries(ini utils)

include_directories(include)
include_directories(module/utils/include)
add_executable(test test/test.cpp)
target_link_libraries(test ini utils)
