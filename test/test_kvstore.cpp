#include "kvstore.h"

#include <assert.h>

KVStore kvs;


void test_open_store() {

    StoreOptions storeOp;

    assert(!std::filesystem::exists(kvs.getDataDir()));

    KVStoreHandle h = kvs.openStore("test_open_store/", storeOp);

    assert(std::filesystem::exists(kvs.getDataDir()));

    assert(kvs.getActiveFilestream().good());
    assert(kvs.getActiveFilestream().is_open());

    std::filesystem::remove_all(kvs.getDataDir());
    return;
}

// Want to test with threads whether we can write to a locked dir.
// Simulate two processes.
void test_put() {

    StoreOptions sOp;

    KVStoreHandle stH;

    Record rec = {
        .crc = 0,                  // placeholder — real Bitcask computes this over the rest of the record
        .timeStamp = 1789731459,   // Unix timestamp, e.g. from time(nullptr)
        .keySize = 4,
        .valSize = 7,
        .key = "name",
        .val = "testval",
    };

    kvs.put(stH, rec);

    return;
}

int main() {
    test_open_store();
    test_put();

    return 0;
}