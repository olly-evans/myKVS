cmake_minimum_required(VERSION 3.15)
project(myapp VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(-g -O0 -Wall)
add_compile_definitions(SOURCE_ROOT="${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_library(app_lib STATIC ${SOURCES})
target_include_directories(app_lib PUBLIC ${CMAKE_SOURCE_DIR}/include)

add_executable(myKVS src/main.cpp)
target_link_libraries(myKVS PRIVATE app_lib)

set_target_properties(myKVS PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_include_directories(myKVS PRIVATE include)

enable_testing()
add_subdirectory(test)