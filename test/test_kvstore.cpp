#include "kvstore.h"

#include <assert.h>
#include <thread>

namespace fs = std::filesystem;

struct TempDirCleanup {
    fs::path dir;
    ~TempDirCleanup() { 
        std::error_code ec;
        fs::remove_all(dir, ec); 
    }
};

void test_open_new_store(fs::path dir) {

    KVStore kvs;
    StoreFlags flags;

    flags.datafileExtension = ".data";
    KVStoreHandle stH = kvs.open(dir, flags);

    assert(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    assert(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    assert(kvs.getActiveFilestream().good());       /* No error state in stream. */
    assert(kvs.getActiveFilestream().is_open());    /* File should be open. */

    assert(stH.getActiveFileID() == 0);             /* No puts so id should default to zero. */

    assert(kvs.getActiveDatafilePath() == 
           dir / "0.aol.data");
}

void test_open_existing_store(fs::path dir) {
    
    KVStore kvs;

    fs::path mFilePath1 = dir / "1.aol.log";
    std::ofstream mockFile1(mFilePath1);
    mockFile1.close();

    fs::path mFilePath2 = dir / "2.aol.log";
    std::ofstream mockFile2(mFilePath2);
    mockFile2.close();

    fs::path mFilePath3 = dir / "3.aol.log";
    std::ofstream mockFile3(mFilePath3);
    mockFile3.close();
    
    StoreFlags flags;
    flags.datafileExtension = ".log";
    KVStoreHandle stH = kvs.open(dir, flags);

    assert(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    assert(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    assert(kvs.getActiveFilestream().good());       /* No error state in stream. */
    assert(kvs.getActiveFilestream().is_open());    /* File should be open. */

    assert(stH.getActiveFileID() == 3);           
    assert(kvs.getActiveDatafilePath() == 
           dir / "3.aol.log");
    
    
    fs::remove(mFilePath1);
    fs::remove(mFilePath2);
    fs::remove(mFilePath3);
}

/* Perhaps make a test flag where we write in hex to check against. */

void test_put(fs::path dir) {

    KVStore kvs;

    StoreFlags flags; // make this a bitmask.

    KVStoreHandle stH = kvs.open(dir, flags);

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

    fs::remove(dir / "0.aol.data");
}

void test_put_threads_task(KVStore& kvs, KVStoreHandle& stH, uint64_t startID, uint64_t count) {

    for (uint64_t i = startID; i < startID + count; ++i) {
        kvs.put(stH, std::to_string(i), std::to_string(i));
    }
}

void test_put_threads(fs::path dir) {

    KVStore kvs;
    StoreFlags flags;

    KVStoreHandle stH = kvs.open(dir, flags);

    std::thread t1(test_put_threads_task, std::ref(kvs), std::ref(stH), 0, 100);
    std::thread t2(test_put_threads_task, std::ref(kvs), std::ref(stH), 100, 100);

    t1.join();
    t2.join();

    assert(kvs.keyDir.size() == 200);

    // CRC check?
    for (auto it = kvs.keyDir.begin(); it != kvs.keyDir.end(); it++) {
        std::string key = it->first;
        std::string val = kvs.get(stH, key);

        std::cout << it->first << kvs.get(stH, it->first) << std::endl;
        assert(key == val); /* Validate all keys and values are the same as they are for this test. */
    }

    fs::remove(dir / "0.aol.data");
}

void test_put_before_open() {
    
    KVStore kvs;
    StoreFlags flags;

    KVStoreHandle stH;
    kvs.put(stH, "testkey", "testvalue");

    assert(kvs.getActiveDatafilePath().empty());
    assert(!kvs.getActiveFilestream().is_open());
    assert(!fs::exists(kvs.getDataDir()));
}

void test_put_roll_over_datafile(fs::path dir) {

    KVStore kvs;
    StoreFlags flags;

    KVStoreHandle stH = kvs.open(dir, flags);

    size_t mockFileSize = 10;
    stH.setMaxDatafileBytes(mockFileSize);

    uint32_t oldDatafileID = stH.getActiveFileID();

    kvs.put(stH, "foo", "bar");

    uint32_t newDatafileID = stH.getActiveFileID();

    assert(oldDatafileID + 1 == newDatafileID);        /* Should have incremented datafile ID by one. */
    
    assert(kvs.getActiveDatafilePath() == dir / "1.aol.data");

    fs::path oldDatafilePath = dir / "0.rol.data";

    assert(fs::exists(oldDatafilePath)); /* Old path exists as read-only. */
    assert(fs::file_size(oldDatafilePath) < stH.getMaxDatafileBytes()); /* Less than max size. */   

    fs::remove(dir / "0.rol.data");
    fs::remove(dir / "1.aol.data");
}

// void test_get(fs::path dir) {

// }

int main() {

    fs::path path = SOURCE_ROOT;
    fs::path testDataDir = path / "test/test_kvstore/";

    if (!fs::exists(testDataDir))
        fs::create_directories(testDataDir);

    test_open_new_store(testDataDir);
    test_open_existing_store(testDataDir);

    // Split into two files I think when bothered, perhaps if 3+ tests each.

    test_put(testDataDir);
    test_put_before_open();
    test_put_roll_over_datafile(testDataDir);
    test_put_threads(testDataDir);


    return 0;
}