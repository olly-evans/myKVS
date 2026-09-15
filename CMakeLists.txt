cmake_minimum_required(VERSION 3.15)
project(myapp VERSION 0.0.1 LANGUAGES CXX)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(app_lib STATIC ${SOURCES})
target_include_directories(app_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(myLSFS src/main.cpp)
target_link_libraries(myLSFS PRIVATE app_lib)

set_property(TARGET myLSFS PROPERTY CXX_STANDARD 17)

target_include_directories(myLSFS PRIVATE include)

enable_testing()
include_directories(test)