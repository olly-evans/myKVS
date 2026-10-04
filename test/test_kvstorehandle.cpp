#include "kvstorehandle.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>


TEST_CASE("KVStoreHandle sets read/write permissions for the store.", "KVStoreHandle::setReadWrite()") {
    
    KVStoreHandle stH;
    bool readWrite = true;
    stH.setReadWrite(readWrite);

    REQUIRE(stH.getReadWrite() == readWrite);
}

TEST_CASE("KVStoreHandle sets a datafile extension for the store", "setDatafileExt()") {
    
    KVStoreHandle stH;
    stH.setDatafileExt(".data");

    REQUIRE(stH.getDatafileExt() == ".data");
}

TEST_CASE("KVStoreHandle updates the active datafile id in datadir", "updateActiveFileID()") {

    KVStoreHandle stH;

    fs::path dataDir = createTempTestDir("test_kvstorehandle_5");
    TempDirGuard cleanup{dataDir};

    stH.setDatafileExt(".data"); // Only have default extension if KVStore::open ran.
    std::ofstream mockFile1 (dataDir / "0.aol.data");
    mockFile1.close();

    std::ofstream mockFile2 (dataDir / "5.aol.data");
    mockFile2.close();

    stH.updateActiveDatafileID(dataDir);

    REQUIRE(stH.getActiveDatafileID() == 5);
}