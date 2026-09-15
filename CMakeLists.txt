cmake_minimum_required(VERSION 3.15)
project(myLSFS VERSION 0.0.1 LANGUAGES CXX)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(myLSFS ${SOURCES})

set_property(TARGET myLSFS PROPERTY CXX_STANDARD 17)

target_include_directories(myLSFS PRIVATE include)