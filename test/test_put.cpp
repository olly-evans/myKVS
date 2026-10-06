#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>
#include <thread>

TEST_CASE("Put writes correct bytes and these bytes can be read back.", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = TestHelpers::create_tmp_dir("test_put_1/");
    TempDirGuard cleanup(dataDir);

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    auto putresult1 = kvs.put(stH, "k", "v"); // Size of all record members, 22 bytes for "k" and "v".
    REQUIRE(putresult1);

    REQUIRE(fs::file_size(stH.getActiveDatafilePath()) == 22); /* Filesize should be 22 bytes after put. */

    auto putresult2 = kvs.put(stH, "k2", "v2"); // 24 bytes.
    REQUIRE(putresult2);

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
        auto putresult = kvs.put(stH, std::to_string(i), std::to_string(i));
        REQUIRE(putresult);
    }
}

TEST_CASE("Threads use put on same kvs", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = TestHelpers::create_tmp_dir("test_put_2/");
    TempDirGuard cleanup(dataDir);

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();


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
    auto putresult = kvs.put(stH, "testkey", "testvalue");
    REQUIRE(!putresult); /* Should return a KVError */

    REQUIRE(stH.getActiveDatafilePath().empty());
    REQUIRE(!kvs.getActiveOutputStream().is_open());
    REQUIRE(!fs::exists(kvs.getDataDir()));
}

TEST_CASE("Rolling over a datafile in put", "KVStore::put()") {

    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = TestHelpers::create_tmp_dir("test_put_3/");
    TempDirGuard cleanup(dataDir);

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    size_t mockFileSize = 10;
    kvs.setMaxDatafileBytes(mockFileSize);

    uint32_t oldDatafileID = stH.getActiveDatafileID();

    auto putresult = kvs.put(stH, "foo", "bar");
    REQUIRE(putresult);

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

    fs::path dataDir = TestHelpers::create_tmp_dir("test_get_4/");
    TempDirGuard cleanup(dataDir);

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    std::string k = "key";
    std::string v = "value";
    std::string newv = "newvalue";

    auto putresult1 = kvs.put(stH, k, v);
    REQUIRE(putresult1);
    
    auto putresult2 = kvs.put(stH, k, newv);
    REQUIRE(putresult2);
    

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

    fs::path dataDir = TestHelpers::create_tmp_dir("test_get_5/");
    TempDirGuard cleanup(dataDir);

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    uint64_t putNum = 32;
    for (uint64_t i = 0; i < putNum; i++) {
        auto putresult = kvs.put(stH, std::to_string(i), std::to_string(i));
        REQUIRE(putresult);
    }

    std::vector<std::string> keys = kvs.listKeys(stH);
    REQUIRE(keys.size() == putNum);
}