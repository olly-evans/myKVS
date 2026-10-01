#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("Put writes correct bytes and these bytes can be read back.", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_put_1/");
    TempDirCleanup cleanup(dataDir);

    KVStoreHandle stH = kvs.open(dataDir, flags);

    kvs.put(stH, "k", "v"); // Size of all record members, 22 bytes for "k" and "v".
    REQUIRE(fs::file_size(kvs.getActiveDatafilePath()) == 22); /* Filesize should be 22 bytes after put. */

    kvs.put(stH, "k2", "v2"); // 24 bytes.
    REQUIRE(fs::file_size(kvs.getActiveDatafilePath()) == 46); /* Filesize should be 46 bytes after put. */
    
    std::ifstream readDatafileStream(kvs.getActiveDatafilePath(), std::ios::binary | std::ios::in);

    KeyDirEntry entry = KVStoreTestAccess::keyDir(kvs).at("k2"); 
    readDatafileStream.seekg(entry.valFileOffset, std::ios_base::beg);

    std::string rdbuf(entry.valSz, '\0');
    readDatafileStream.read(rdbuf.data(), entry.valSz);

    REQUIRE(rdbuf == "v2"); /* Should correctly read the value from the offset in keyDir in df. */

}

void test_put_threads_task(KVStore& kvs, KVStoreHandle& stH, uint64_t startID, uint64_t count) {

    for (uint64_t i = startID; i < startID + count; ++i) {
        kvs.put(stH, std::to_string(i), std::to_string(i));
    }
}

TEST_CASE("Threads use put on same kvs", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_put_1/");
    TempDirCleanup cleanup(dataDir);

    KVStoreHandle stH = kvs.open(dataDir, flags);

    std::thread t1(test_put_threads_task, std::ref(kvs), std::ref(stH), 0, 100);
    std::thread t2(test_put_threads_task, std::ref(kvs), std::ref(stH), 100, 100);

    t1.join();
    t2.join();

    REQUIRE(KVStoreTestAccess::keyDir(kvs).size() == 200);
    
    /* Validate all keys and values are the same as they are for this test. */
    for (auto it = KVStoreTestAccess::keyDir(kvs).begin(); it != KVStoreTestAccess::keyDir(kvs).end(); it++) {
        std::optional<std::string> val = kvs.get(stH, it->first);

        // std::cout << it->first << val->first << std::endl;
        REQUIRE(val != std::nullopt);
        REQUIRE(it->first == val); 
    }
}

void test_put_before_open() {
    
    KVStore kvs;
    StoreFlags flags;

    KVStoreHandle stH;
    kvs.put(stH, "testkey", "testvalue");

    REQUIRE(kvs.getActiveDatafilePath().empty());
    REQUIRE(!kvs.getActiveFilestream().is_open());
    REQUIRE(!fs::exists(kvs.getDataDir()));
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

    REQUIRE(oldDatafileID + 1 == newDatafileID);        /* Should have incremented datafile ID by one. */
    
    REQUIRE(kvs.getActiveDatafilePath() == dir / "1.aol.data");

    fs::path oldDatafilePath = dir / "0.rol.data";

    REQUIRE(fs::exists(oldDatafilePath)); /* Old path exists as read-only. */
    REQUIRE(fs::file_size(oldDatafilePath) < stH.getMaxDatafileBytes()); /* Less than max size. */   

    fs::remove(dir / "0.rol.data");
    fs::remove(dir / "1.aol.data");
}
