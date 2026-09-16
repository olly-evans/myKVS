include(CTest)

add_executable(test_kvstore test_kvstore.cpp)
target_link_libraries(test_kvstore PRIVATE app_lib)
add_test(NAME kvstore_test COMMAND test_kvstore)

add_executable(test_kvstorehandle test_kvstorehandle.cpp)
target_link_libraries(test_kvstorehandle PRIVATE app_lib)
add_test(NAME kvstorehandle_test COMMAND test_kvstorehandle)