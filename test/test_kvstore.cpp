#include "kvstore.h"

#include <assert.h>
#include <iostream>

void test_set_absolute_directory() {

    KVStoreHandle handle;
    handle.setAbsDirPath();

    std::cout << handle.getAbsDirPath() << "\n";

    assert(false);
}

int main() {

    test_set_absolute_directory();

    return 0;
}