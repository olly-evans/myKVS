#include "kvstore.h"

#include <assert.h>
#include <iostream>

void test_open_store() {

    KVStore kvs;

    assert(!std::filesystem::exists(kvs.getDataDir()));
    KVStoreHandle h = kvs.openStore(SOURCE_ROOT);

    assert(h.getActiveFileID() == 0);
    assert(std::filesystem::exists(kvs.getDataDir()));


    return;
}

int main() {
    test_open_store();

    return 0;
}