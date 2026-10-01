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

    uint64_t numPuts = 64;

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

TEST_CASE("test get doesn't return corrupted data.", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_3/");
    TempDirGuard cleanup{dataDir};

    KVStoreHandle stH = kvs.open(dataDir, flags);
    kvs.put(stH, "key", "val");
   
    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at("key");

    std::optional<std::string> noncorruptval = kvs.get(stH, "key");

    std::string corrupt = "Xp";

    // std::ios::out on its own usually means a new file hence it truncates. Need std::ios::in too.
    std::ofstream f(kvs.getActiveDatafilePath(), std::ios::in  | 
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

