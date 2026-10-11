#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Does restore reboot for one put", "KVStore::restore()") {
    
    TempDirGuard guard(TestHelpers::create_tmp_dir());

    KVStore kvs(guard.dir);
    StoreFlags flags;

    fs::path mFilePath1 = guard.dir/ "0.aol.data";
    std::ofstream mockFile1(mFilePath1);

    Record rec("key", "val");

    // Simulating a put.
    kvs.setActiveOutputStream(std::move(mockFile1));
    rec.serialize(kvs.getActiveOutputStream());

    kvs.getActiveOutputStream().close();

    // Note: We aren't updating the keyDir here as we want to restore it.

    KVExpected result = kvs.open(flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();
    
    REQUIRE(HandleTestAccess::keyDir(stH).size() == 1);

    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at("key");
    REQUIRE(entry.fileID == 0);
    REQUIRE(entry.tstamp == rec.getTimestamp());
    REQUIRE(entry.valFileOffset == 23);
    REQUIRE(entry.valSz == 3);
}

TEST_CASE("Does restore work for an existing store with a series of previous puts", "KVStore::restore()") {
        
    TempDirGuard guard(TestHelpers::create_tmp_dir());

    KVStore kvs(guard.dir);
    StoreFlags flags;

    flags.maxDatafileBytes = 30; /* Put sizes below both 30. */

    KVExpected openresult = kvs.open(flags);
    
    KVResult put1 = kvs.put(openresult.value(), "key1", "value1");
    KVResult put2 = kvs.put(openresult.value(), "key2", "value2");

    REQUIRE(openresult.value().getActiveDatafileID() == 1); 

    KVStore restorekvs(guard.dir);
    KVExpected restoreresult = restorekvs.open(flags);
    KVStoreHandle stH = restoreresult.value();

    REQUIRE(stH.getActiveDatafileID() == 1);
    REQUIRE(HandleTestAccess::keyDir(stH).size() == 2);

    KVResult getresult1 = restorekvs.get(stH, "key1");
    REQUIRE(getresult1.value() == "value1");
    
    KVResult getresult2 = restorekvs.get(stH, "key2");
    REQUIRE(getresult2.value() == "value2");

}

