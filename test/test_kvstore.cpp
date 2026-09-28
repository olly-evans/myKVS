#include "kvstore.h"

#include <assert.h>

namespace fs = std::filesystem;

struct TempDirCleanup {
    fs::path dir;
    ~TempDirCleanup() { 
        std::error_code ec;
        fs::remove_all(dir, ec); 
    }
};

void test_open_new_store() {

    KVStore kvs;
    StoreFlags flags;

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_open_new_store/";
    fs::create_directories(dataDir);
    
    TempDirCleanup cleanup{dataDir};

    flags.datafileExtension = ".data";
    KVStoreHandle stH = kvs.open(dataDir, flags);

    assert(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    assert(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    assert(kvs.getActiveFilestream().good());       /* No error state in stream. */
    assert(kvs.getActiveFilestream().is_open());    /* File should be open. */

    assert(stH.getActiveFileID() == 0);             /* No puts so id should default to zero. */

    assert(kvs.getActiveDatafilePath() == 
           dataDir / "0.aol.data");

    return;
}

void test_open_existing_store() {
    
    KVStore kvs;

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_open_existing/";
    fs::create_directories(dataDir);
    
    TempDirCleanup tdCleanup{dataDir};

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
    
    return;

}

/* Perhaps make a test flag where we write in hex to check against. */

void test_put() {

    KVStore kvs;

    fs::path path = SOURCE_ROOT;
    fs::path dataDir = path / "test_put/";
    fs::create_directories(dataDir);

    TempDirCleanup tdCleanup{dataDir};

    StoreFlags flags; // make this a bitmask.

    KVStoreHandle stH = kvs.open(dataDir, flags);

    kvs.put(stH, "k", "v"); // Size of all record members, 22 bytes for "k" and "v".
    assert(fs::file_size(kvs.getActiveDatafilePath()) == 22); /* Filesize should be 22 bytes after put. */

    kvs.put(stH, "k2", "v2"); // 24 bytes.
    assert(fs::file_size(kvs.getActiveDatafilePath()) == 46); /* Filesize should be 46 bytes after put. */
    

    std::ifstream readDatafileStream(kvs.getActiveDatafilePath(), std::ios::binary | std::ios::in);

    KeyDirEntry entry = kvs.keyDir.at("k2"); 
    readDatafileStream.seekg(entry.offset, std::ios_base::beg);

    std::string rdbuf(entry.valSz, '\0');
    readDatafileStream.read(rdbuf.data(), entry.valSz);

    assert(rdbuf == "v2"); /* Should correctly read the value from the offset in keyDir in df. */

    return;
}

int main() {

    test_open_new_store();
    test_open_existing_store();
    test_put();

    return 0;
}