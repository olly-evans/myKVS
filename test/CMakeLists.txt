include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

find_package(Threads REQUIRED)

add_executable(kvstore_tests test_kvstore.cpp)    # your test file(s)

target_link_libraries(kvstore_tests PRIVATE
    app_lib
    Catch2::Catch2WithMain
    Threads::Threads
)

include(Catch)
catch_discover_tests(kvstore_tests)