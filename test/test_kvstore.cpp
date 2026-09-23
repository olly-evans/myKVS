#include "kvstore.h"

#include <assert.h>

void test_open() {
    
    namespace fs = std::filesystem;

    KVStore kvs;
    StoreFlags flags;

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_kvstore/";
    fs::create_directories(dataDir);
    
    KVStoreHandle stH = kvs.open(dataDir, flags);

    /* After openStore() call we should have an existing datastore. */
    assert(fs::exists(kvs.getDataDir()));

    /* Datafile extension cannot be empty. */
    assert(!stH.getDatafileExt().empty());

    /* No puts so id should be zero. */
    assert(stH.getActiveFileID() == 0);

    /* No error state in stream. */
    assert(kvs.getActiveFilestream().good());

    /* File should be open. */
    assert(kvs.getActiveFilestream().is_open());
    
    fs::remove_all(dataDir);

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


    test_open();
    // test_create_set_new_datafile();

    // test_put();


    return 0;
}