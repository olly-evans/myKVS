#include "kvstore.h"

#include <assert.h>

void test_open_store() {

    KVStore kvs;

    assert(!std::filesystem::exists(kvs.getDataDir()));

    KVStoreHandle h = kvs.openStore(SOURCE_ROOT);

    assert(std::filesystem::exists(kvs.getDataDir()));
    assert(h.getActiveFileID() == 0);

    return;
}

int main() {
    test_open_store();

    return 0;
}