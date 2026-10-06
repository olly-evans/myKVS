#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Does restore reboot for one put", "KVStore::restore()") {
    
    KVStore kvs;
    StoreFlags flags;

    fs::path dataDir = createTempTestDir("test_restore_1/");
    TempDirGuard cleanup(dataDir);

    fs::path mFilePath1 = dataDir / "0.aol.data";
    std::ofstream mockFile1(mFilePath1);

    Record rec("key", "val");

    // Simulating a put.
    kvs.setActiveOutputStream(std::move(mockFile1));
    rec.serialize(kvs.getActiveOutputStream());

    kvs.getActiveOutputStream().close();

    // Note: We aren't updating the keyDir here as we want to restore it.

    KVResult result = kvs.open(dataDir, flags);
    KVStoreHandle stH = result.value();
    
    REQUIRE(HandleTestAccess::keyDir(stH).size() == 1);

    KeyDirEntry entry = HandleTestAccess::keyDir(stH).at("key");
    REQUIRE(entry.fileID == 0);
    REQUIRE(entry.tstamp == rec.getTimestamp());
    REQUIRE(entry.valFileOffset == 23);
    REQUIRE(entry.valSz == 3);
}