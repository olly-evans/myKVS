#include "kvstore.h"

#include <assert.h>
namespace fs = std::filesystem;

void test_open_new_store() {
    

    KVStore kvs;
    StoreFlags flags;

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_open_initial/";
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

void test_open_existing_store() {
    
    KVStore kvs;


    // DIFFERENT FILE EXTENSIONS??!
    // ACTIVEID DOESNT CARE.

    // IF YOU RESTORE YOU CANNOT CHANGE DATAFILE EXTENSION.

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_open_restore/";
    fs::create_directories(dataDir);
    
    std::ofstream mockFile1(dataDir / "0.aol.log");
    mockFile1.close();

    std::ofstream mockFile2(dataDir / "1.aol.log");
    mockFile2.close();

    std::ofstream mockFile3(dataDir / "2.aol.log");
    mockFile3.close();
    
    StoreFlags flags;
    flags.datafileExtension = ".log";
    KVStoreHandle stH = kvs.open(dataDir, flags);

    /* After openStore() call we should have an existing datastore. */
    assert(fs::exists(kvs.getDataDir()));

    /* Datafile extension cannot be empty. */
    assert(!stH.getDatafileExt().empty());

    /* No puts so id should be zero. */
    assert(stH.getActiveFileID() == 2);

    /* No error state in stream. */
    assert(kvs.getActiveFilestream().good());

    /* File should be open. */
    assert(kvs.getActiveFilestream().is_open());

    assert(kvs.getActiveDatafilePath() == dataDir / "2.aol.log");
    
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

int main() {

    test_open_new_store();
    test_open_existing_store();

    return 0;
}