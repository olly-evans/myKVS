include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

find_package(Threads REQUIRED)

file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(tests ${TESTS})

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/test_include)

target_link_libraries(tests PRIVATE
    app_lib
    Catch2::Catch2WithMain
    Threads::Threads
)

include(Catch)
catch_discover_tests(tests)