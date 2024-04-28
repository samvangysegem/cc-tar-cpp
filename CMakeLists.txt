cmake_minimum_required(VERSION 3.20)

project(cc-tar
        VERSION 0.1.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Package manager
include(cmake/CPM.cmake)

CPMAddPackage(
        NAME
        Catch2
        GIT_TAG
        v3.5.3
        GITHUB_REPOSITORY
        catchorg/Catch2
)

# Libraries
add_subdirectory(lib/error)
add_subdirectory(lib/program_options)

# Main program
add_executable(cc-tar 
        main.cpp 
        src/file_handler.cpp
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
        program_options
)

target_compile_options(cc-tar 
        PUBLIC
        -Wall
        -Wfloat-conversion
)

# Unit tests
add_executable(cc-tar-tests 
        test/test.cpp
        src/detail.cpp
)

target_include_directories(cc-tar-tests
        PRIVATE
        include
        src/internal
)

target_link_libraries(cc-tar-tests
        PRIVATE
        boost_leaf
        program_options
        Catch2::Catch2WithMain
)

