include(CTest)

add_executable(test_client test_client.cpp)
target_link_libraries(test_client PRIVATE app_lib)
add_test(NAME client_test COMMAND test_client)