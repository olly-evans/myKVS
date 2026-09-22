#include "kvstore.h"

#include <assert.h>

void test_open_store(KVStore& kvs, KVStoreHandle& stH) {
    
    /* After openStore() call we should have an existing datastore. */
    assert(std::filesystem::exists(kvs.getDataDir()));

    /* Datafile extension cannot be empty. */
    assert(!stH.getDatafileExt().empty());

    /* No puts so id should be zero. */
    assert(stH.getActiveFileID() == 0);

    /* No error state in stream. */
    assert(kvs.getActiveFilestream().good());

    /* File should be open. */
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

void test_close() {
    return;
}

int main() {

    KVStore kvs;
    
    StoreFlags flags;

    std::filesystem::path path = SOURCE_ROOT;
    std::filesystem::path dataDir = path / "test_kvstore/";
    std::filesystem::create_directories(dataDir);
    
    KVStoreHandle stH = kvs.openStore(dataDir, flags);

    test_open_store(kvs, stH);
    // test_put(kvs, stH);

    std::filesystem::remove_all(dataDir);

    return 0;
}