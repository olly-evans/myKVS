include(CTest)

add_executable(test_kvstore test_kvstore.cpp)
target_link_libraries(test_kvstore PRIVATE app_lib)
add_test(NAME kvstore_test COMMAND test_kvstore)