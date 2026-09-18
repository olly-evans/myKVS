#include "kvstore.h"

#include <assert.h>

void test_open_store(KVStore& kvs, KVStoreHandle& stH) {

    // assert(!std::filesystem::exists(kvs.getDataDir()));

    assert(std::filesystem::exists(kvs.getDataDir()));

    assert(kvs.getActiveFilestream().good());
    assert(kvs.getActiveFilestream().is_open());

    return;
}

// Want to test with threads whether we can write to a locked dir.
// Simulate two processes.
void test_put(KVStore& kvs, KVStoreHandle& stH) {

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

    KVStore kvs;
    StoreOptions stOp;

    std::filesystem::path dataDir = "test_kvstore/";
    KVStoreHandle stH = kvs.openStore(dataDir, stOp);


    test_open_store(kvs, stH);
    test_put(kvs, stH);

    std::filesystem::remove_all(dataDir);

    return 0;
}