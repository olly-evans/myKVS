#include "kvstore.h"

#include <assert.h>
namespace fs = std::filesystem;

void test_open_new_store() {
    

    KVStore kvs;
    StoreFlags flags;

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_open_initial/";
    fs::create_directories(dataDir);

    flags.datafileExtension = ".data";
    KVStoreHandle stH = kvs.open(dataDir, flags);

    assert(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    assert(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    assert(kvs.getActiveFilestream().good());       /* No error state in stream. */
    assert(kvs.getActiveFilestream().is_open());    /* File should be open. */

    assert(stH.getActiveFileID() == 0);             /* No puts so id should be zero. */

    assert(kvs.getActiveDatafilePath() == 
           dataDir / "0.aol.data");

    fs::remove_all(dataDir);

    return;
}

void test_open_existing_store() {
    
    KVStore kvs;

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

    assert(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    assert(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    assert(kvs.getActiveFilestream().good());       /* No error state in stream. */
    assert(kvs.getActiveFilestream().is_open());    /* File should be open. */

    assert(stH.getActiveFileID() == 2);           
    assert(kvs.getActiveDatafilePath() == 
           dataDir / "2.aol.log");
    
    fs::remove_all(dataDir);

    return;

}

/* Perhaps make a test flag where we write in hex to check against. */

void test_put() {

    KVStore kvs;

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_put/";
    fs::create_directories(dataDir);

    StoreFlags flags; // make this a bitmask.

    KVStoreHandle stH = kvs.open(dataDir, flags);

    kvs.put(stH, "k", "v"); // 22 bytes.
    
    assert(fs::file_size(kvs.getActiveDatafilePath()) > 0);
    assert(fs::file_size(kvs.getActiveDatafilePath()) == 22);

    kvs.put(stH, "k2", "v2"); // 24 bytes.
    assert(fs::file_size(kvs.getActiveDatafilePath()) == 46);
    return;
}

int main() {

    test_open_new_store();
    test_open_existing_store();
    test_put();

    return 0;
}