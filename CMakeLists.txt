cmake_minimum_required(VERSION 3.20)
project(cc-tar)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)

add_executable(cc-tar main.cpp 
                      src/parser.cpp
                      src/detail.cpp)

target_include_directories(cc-tar 
                        PUBLIC 
                        include 
                        PRIVATE 
                        src/internal)

target_compile_options(cc-tar 
                        PUBLIC
                        -Wall
                        -Wfloat-conversion)
