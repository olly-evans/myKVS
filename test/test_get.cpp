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

TEST_CASE("test get doesn't return corrupted data.", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_get_2/");
    TempDirGuard cleanup{dataDir};

    KVStoreHandle stH = kvs.open(dataDir, flags);
    kvs.put(stH, "key", "val");
   
    KeyDirEntry entry = KVStoreTestAccess::keyDir(kvs).at("key");

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

TEST_CASE("Get responds appropriately to a key collision in the keydir", "KVStore::get()") {
    return;
}