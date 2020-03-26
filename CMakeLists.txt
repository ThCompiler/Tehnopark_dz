cmake_minimum_required(VERSION 3.15)
project(Process)

include_directories(include)

add_library(Process_lib STATIC src/Process.cpp)

add_executable(Process src/main.cpp)

target_link_libraries(Process PUBLIC Process_lib)
