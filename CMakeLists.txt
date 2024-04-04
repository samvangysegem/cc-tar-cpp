cmake_minimum_required(VERSION 3.20)

project(cc-tar
        VERSION 0.1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Package manager
include(cmake/CPM.cmake)

CPMAddPackage(
        NAME
        boost-leaf
        GIT_TAG
        boost-1.82.0
        GITHUB_REPOSITORY
        boostorg/leaf
)

# Main program
add_executable(cc-tar 
        main.cpp 
        src/parser.cpp
        src/detail.cpp
)

target_include_directories(cc-tar 
        PUBLIC 
        include 
        PRIVATE 
        src/internal
)

target_link_libraries(cc-tar 
        PRIVATE 
        boost_leaf
)

target_compile_options(cc-tar 
        PUBLIC
        -Wall
        -Wfloat-conversion
)

target_compile_definitions(cc-tar 
        PRIVATE 
        DEBUG
)
