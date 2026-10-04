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

    kvs.setActiveOutputStream(std::move(mockFile1));
    rec.serialize(kvs.getActiveOutputStream());

    mockFile1.close();

    // Note: We aren't updating the keyDir here as we want to restore it.

    KVStoreHandle stH = kvs.open(dataDir, flags);

}