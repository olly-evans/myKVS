include(FetchContent)
FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.7.1
)
FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)

find_package(Threads REQUIRED)

add_executable(tests test_open.cpp test_put.cpp test_get.cpp test_kvstorehandle.cpp test_record.cpp testing_helpers.cpp)

target_link_libraries(tests PRIVATE
    app_lib
    Catch2::Catch2WithMain
    Threads::Threads
)

include(Catch)
catch_discover_tests(tests)