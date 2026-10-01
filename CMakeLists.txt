cmake_minimum_required(VERSION 3.15)
project(myapp VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(-g -O0 -Wall)
add_compile_definitions(SOURCE_ROOT="${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "src/*.cpp")
list(REMOVE_ITEM SOURCES ${CMAKE_SOURCE_DIR}/src/main.cpp)

add_library(app_lib STATIC ${SOURCES})
target_include_directories(app_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(myKVS src/main.cpp)
target_link_libraries(myKVS PRIVATE app_lib)

enable_testing()
add_subdirectory(test)