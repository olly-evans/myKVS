include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

find_package(Threads REQUIRED)

# helpers: anything that isn't a test itself
add_library(testing_helpers STATIC
    ${CMAKE_CURRENT_SOURCE_DIR}/testing_helpers.cpp)
target_include_directories(testing_helpers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/test_include)
target_link_libraries(testing_helpers PUBLIC app_lib)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

include(Catch)

foreach(test_src IN LISTS TEST_SOURCES)
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_include_directories(${test_name} PUBLIC ${CMAKE_SOURCE_DIR}/include)
    target_link_libraries(${test_name} PRIVATE
        testing_helpers
        Catch2::Catch2WithMain
        Threads::Threads
    )
    catch_discover_tests(${test_name})
endforeach()