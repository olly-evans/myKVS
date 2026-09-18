#include "kvstore.h"

#include <assert.h>

void test_open_store(KVStore& kvs, KVStoreHandle& stH) {
    assert(std::filesystem::exists(kvs.getDataDir()));

    assert(kvs.getActiveFilestream().good());
    assert(kvs.getActiveFilestream().is_open());

    return;
}

void test_put(KVStore& kvs, const KVStoreHandle& stH) {

    // Record rec = {
    //     .crc = 0,                  // placeholder — real Bitcask computes this over the rest of the record
    //     .timeStamp = 1789731459,   // Unix timestamp, e.g. from time(nullptr)
    //     .keySize = 4,
    //     .valSize = 7,
    //     .key = "name",
    //     .val = "testval",
    // };

    // kvs.put(stH, rec);

    return;
}

int main() {

    KVStore kvs;
    StoreOptions sOp;

    std::filesystem::path dataDir = "test_kvstore/";
    KVStoreHandle stH = kvs.openStore(dataDir, sOp);

    test_open_store(kvs, stH);
    test_put(kvs, stH);

    std::filesystem::remove_all(dataDir);

    return 0;
}