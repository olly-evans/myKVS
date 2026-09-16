#include "kvstore.h"

#include <assert.h>
#include <iostream>

void test_kvshandle_sets_absolute_directory() {

    KVStoreHandle handle;
    handle.setAbsDirPath();

    assert(std::filesystem::exists(handle.getAbsDirPath()));
}

void test_open_store() {

    KVStore kvs;

    KVStoreHandle h = kvs.openStore(SOURCE_ROOT);

    assert(h.getActiveFileID());
    assert(std::filesystem::exists(kvs.getDataDir()));

    std::cout << kvs.getDataDir() << "\n";
    return;
}

int main() {

    test_kvshandle_sets_absolute_directory();
    test_open_store();

    return 0;
}