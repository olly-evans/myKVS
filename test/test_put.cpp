#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("Put writes correct bytes and these bytes can be read back.", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_put_1/");
    TempDirGuard cleanup(dataDir);

    KVStoreHandle stH = kvs.open(dataDir, flags);

    kvs.put(stH, "k", "v"); // Size of all record members, 22 bytes for "k" and "v".
    REQUIRE(fs::file_size(stH.getActiveDatafilePath()) == 22); /* Filesize should be 22 bytes after put. */

    kvs.put(stH, "k2", "v2"); // 24 bytes.
    REQUIRE(fs::file_size(stH.getActiveDatafilePath()) == 46); /* Filesize should be 46 bytes after put. */
    
    std::ifstream readDatafileStream(stH.getActiveDatafilePath(), std::ios::binary | std::ios::in);

    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at("k2"); 
    readDatafileStream.seekg(static_cast<std::streamoff>(entry.valFileOffset), std::ios_base::beg);

    std::string rdbuf(entry.valSz, '\0');
    readDatafileStream.read(rdbuf.data(), entry.valSz);

    REQUIRE(rdbuf == "v2"); /* Should correctly read the value from the offset in keyDir in df. */

}

void simulate_puts_in_range(KVStore& kvs, KVStoreHandle& stH, uint64_t startID, uint64_t count) {

    for (uint64_t i = startID; i < startID + count; ++i) {
        kvs.put(stH, std::to_string(i), std::to_string(i));
    }
}

TEST_CASE("Threads use put on same kvs", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_put_2/");
    TempDirGuard cleanup(dataDir);

    KVStoreHandle stH = kvs.open(dataDir, flags);

    std::thread t1(simulate_puts_in_range, std::ref(kvs), std::ref(stH), 0, 100);
    std::thread t2(simulate_puts_in_range, std::ref(kvs), std::ref(stH), 100, 100);

    t1.join();
    t2.join();

    REQUIRE(HandleTestAccess::keyDir(stH).size() == 200);
    
    /* Validate all keys and values are the same as they are for this test. */
    
    for (auto it = HandleTestAccess::keyDir(stH).begin(); it != HandleTestAccess::keyDir(stH).end(); it++) {
        std::optional<std::string> val = kvs.get(stH, it->first);

        // std::cout << it->first << val->first << std::endl;
        REQUIRE(val != std::nullopt);
        REQUIRE(it->first == val); 
    }
}

TEST_CASE("Put before opening a store.", "KVStore::put()") {
    
    KVStore kvs;
    StoreFlags flags;

    KVStoreHandle stH;
    kvs.put(stH, "testkey", "testvalue");

    REQUIRE(stH.getActiveDatafilePath().empty());
    REQUIRE(!kvs.getActiveOutputStream().is_open());
    REQUIRE(!fs::exists(kvs.getDataDir()));
}

TEST_CASE("Rolling over a datafile in put", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_put_3/");
    TempDirGuard cleanup(dataDir);

    KVStoreHandle stH = kvs.open(dataDir, flags);

    size_t mockFileSize = 10;
    kvs.setMaxDatafileBytes(mockFileSize);

    uint32_t oldDatafileID = stH.getActiveDatafileID();

    kvs.put(stH, "foo", "bar");

    uint32_t newDatafileID = stH.getActiveDatafileID();

    REQUIRE(oldDatafileID + 1 == newDatafileID);        /* Should have incremented datafile ID by one. */
    
    REQUIRE(stH.getActiveDatafilePath() == dataDir / "1.aol.data");

    fs::path oldDatafilePath = dataDir / "0.rol.data";

    REQUIRE(fs::exists(oldDatafilePath)); /* Old path exists as read-only. */
    REQUIRE(fs::file_size(oldDatafilePath) < kvs.getMaxDatafileBytes()); /* Less than max size. */   
}

TEST_CASE("Put responds appropriately to a key collision in the keydir", "KVStore::put()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_4/");
    TempDirGuard cleanup(dataDir);

    KVStoreHandle stH = kvs.open(dataDir, flags);

    std::string k = "key";
    std::string v = "value";
    std::string newv = "newvalue";

    kvs.put(stH, k, v);
    kvs.put(stH, k, newv);

    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at(k);

    Record rec1(k, v);
    Record rec2(k, newv);
    
    size_t expectedDatafileSize = rec1.byteSize() + rec2.byteSize();
    REQUIRE(fs::file_size(stH.getActiveDatafilePath()) == expectedDatafileSize);

    REQUIRE(entry.valSz == 8);
    REQUIRE(HandleTestAccess::keyDir(stH).size() == 1);

    REQUIRE(kvs.get(stH, k) == newv);
}

TEST_CASE("listKeys returns vector of appropriate size", "KVStore::listKeys()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_5/");
    TempDirGuard cleanup(dataDir);

    KVStoreHandle stH = kvs.open(dataDir, flags);

    uint64_t putNum = 32;
    for (uint64_t i = 0; i < putNum; i++) {
        kvs.put(stH, std::to_string(i), std::to_string(i));
    }

    std::vector<std::string> keys = kvs.listKeys(stH);
    REQUIRE(keys.size() == putNum);
}