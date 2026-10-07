#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Does get retrieve one value correctly", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = TestHelpers::create_tmp_dir("test_get_1/");
    TempDirGuard cleanup{dataDir};

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    auto putresult = kvs.put(stH, "key", "val");
    REQUIRE(putresult);

    std::optional<std::string> val = kvs.get(stH, "key");

    Record rec("key", "val");

    REQUIRE(val.has_value());
    REQUIRE(val.value() == "val");
}

TEST_CASE("Does get retrieve multiple values correctly", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = TestHelpers::create_tmp_dir("test_get_2/");
    TempDirGuard cleanup{dataDir};

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    uint64_t numPuts = 64;

    for (uint64_t i = 0; i < numPuts; i++) {
        auto putresult = kvs.put(stH, std::to_string(i), std::to_string(i));
        REQUIRE(putresult);
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

// TEST_CASE(Does get change input stream correctly. )


TEST_CASE("Opened store and put. Opened store again and get works.", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = TestHelpers::create_tmp_dir("test_get_3/");
    TempDirGuard cleanup{dataDir};

    TestHelpers::create_mock_aol_file(dataDir, ".data", 1);

    KVExpected openresult1 = kvs.open(dataDir, flags);
    REQUIRE(openresult1);
    KVStoreHandle& stH = openresult1.value();

    auto putresult = kvs.put(stH, "key", "val"); /* Put to mockFile1 */
    REQUIRE(putresult);

    stH.rollOverDatafile(dataDir, kvs.getActiveOutputStream());

    KVStore newkvs;

    KVExpected openresult2 = newkvs.open(dataDir, flags); /* activeInputStream with mf2. */
    REQUIRE(openresult2);
    stH = openresult2.value();

    std::optional<std::string> val = newkvs.get(stH, "key");

    REQUIRE(val.value() == "val"); 
}

TEST_CASE("test get doesn't return corrupted data.", "KVStore::get()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = TestHelpers::create_tmp_dir("test_get_4/");
    TempDirGuard cleanup{dataDir};

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

    auto putresult = kvs.put(stH, "key", "val");
    REQUIRE(putresult);
    
    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at("key");

    std::optional<std::string> noncorruptval = kvs.get(stH, "key");

    std::string corrupt = "Xp";

    // std::ios::out on its own usually means a new file hence it truncates. Need std::ios::in too.
    std::ofstream in(stH.getActiveDatafilePath(), std::ios::in | 
                                                 std::ios::out | 
                                                 std::ios::binary);

    in.seekp(entry.valFileOffset, std::ios_base::beg);
    in.write(corrupt.data(), corrupt.size());
    in.close();

    std::optional<std::string> corruptval = kvs.get(stH, "key");

    REQUIRE(corruptval == std::nullopt);    /* val being std::nullopt after corruption implies computed and 
                                              read crc values are different, desired. */

    REQUIRE(noncorruptval == "val");        /* Before corruption read should be original value, "val" */
}

