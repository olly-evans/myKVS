#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Does get retrieve one value correctly", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_1/");
    TempDirGuard cleanup{dataDir};

    KVStoreHandle stH = kvs.open(dataDir, flags);

    kvs.put(stH, "key", "val");

    std::optional<std::string> val = kvs.get(stH, "key");

    Record rec("key", "val");

    REQUIRE(val.has_value());
    REQUIRE(val.value() == "val");
}

TEST_CASE("Does get retrieve multiple values correctly", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_2/");
    TempDirGuard cleanup{dataDir};

    KVStoreHandle stH = kvs.open(dataDir, flags);

    uint64_t numPuts = 1;

    for (uint64_t i = 0; i < numPuts; i++) {
        kvs.put(stH, std::to_string(i), std::to_string(i));
    }
    
    REQUIRE(HandleTestAccess::keyDir(stH).size() == numPuts);

    for (uint64_t i = 0; i < numPuts; i++) {

        std::string key = std::to_string(i);
        std::optional<std::string> val = kvs.get(stH, key);
        Record rec(key, val.value());

        REQUIRE(val != std::nullopt);
        REQUIRE(key == val);
    }

}

TEST_CASE("Does get change input stream correctly. Opened store and put. Opened store again and read.", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_3/");
    TempDirGuard cleanup{dataDir};

    fs::path mFilePath1 = dataDir / "1.aol.data";
    std::ofstream mockFile1(mFilePath1);
    mockFile1.close();

    KVStoreHandle stH = kvs.open(dataDir, flags);
    REQUIRE(stH.getActiveDatafileID() == 1);
    kvs.put(stH, "key", "val"); /* Put to mockFile1 */

    REQUIRE(kvs.getActiveInputStream().is_open());
    REQUIRE(kvs.getActiveInputStream().good());

    fs::path mFilePath2 = dataDir / "2.aol.data";
    std::ofstream mockFile2(mFilePath2);
    mockFile2.close();

    KVStore kvs2; /* NEW KEYDIR*/

    /* SO FOR SOME REASON WHEN WE OPEN NEW STORE A CRC CHECK IS FAILING. */
    stH = kvs2.open(dataDir, flags); /* activeInputStream with mf2. */

    REQUIRE(stH.getActiveDatafileID() == 2);
    REQUIRE(kvs2.getActiveInputStream().is_open());
    REQUIRE(kvs2.getActiveInputStream().good());

    fs::path readPathBeforeGet = kvs2.getActiveInputStreamPath(); /* mf2 */
    std::optional<std::string> val = kvs2.get(stH, "key");
    fs::path readPathAfterGet = kvs2.getActiveInputStreamPath(); /* also mf2.... */

    REQUIRE(val.value() == "val"); // failing.
    

    REQUIRE(kvs2.getActiveInputStream().is_open());
    REQUIRE(kvs2.getActiveInputStream().good()); /* perhaps need a .clear(), this is a new stream now remember, new object.*/

    REQUIRE(readPathBeforeGet != readPathAfterGet); /* Should be different as defaults to mf2, 
                                                       then puts to mf1, reads from it. */
}

TEST_CASE("test get doesn't return corrupted data.", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_4/");
    TempDirGuard cleanup{dataDir};

    KVStoreHandle stH = kvs.open(dataDir, flags);
    kvs.put(stH, "key", "val");
   
    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at("key");

    std::optional<std::string> noncorruptval = kvs.get(stH, "key");

    std::string corrupt = "Xp";

    // std::ios::out on its own usually means a new file hence it truncates. Need std::ios::in too.
    std::ofstream f(stH.getActiveDatafilePath(), std::ios::in  | 
                                                 std::ios::out | 
                                                 std::ios::binary);

    f.seekp(entry.valFileOffset, std::ios_base::beg);
    f.write(corrupt.data(), 2);
    f.close();

    std::optional<std::string> corruptval = kvs.get(stH, "key");

    REQUIRE(corruptval == std::nullopt);    /* val being std::nullopt after corruption implies computed and 
                                              read crc values are different, desired. */

    REQUIRE(noncorruptval == "val");        /* Before corruption read should be original value, "val" */
}

