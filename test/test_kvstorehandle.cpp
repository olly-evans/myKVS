#include "kvstorehandle.h"
#include "testing_helpers.h"

#include <fstream>
#include <iostream>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("KVStoreHandle sets the absolute directory") {

    KVStoreHandle stH;
    stH.setAbsDirPath();

    REQUIRE(std::filesystem::exists(stH.getAbsDirPath()));
}

TEST_CASE("KVStoreHandle updates the active datafile id in datadir", "updateActiveFileID()") {

    KVStoreHandle stH;

    fs::path dataDir = createTempTestDir("test_kvstorehandle_2");
    TempDirGuard cleanup{dataDir};

    stH.setDatafileExt(".data"); // Only have default extension if KVStore::open ran.
    std::ofstream mockFile1 (dataDir / "0.aol.data");
    mockFile1.close();

    std::ofstream mockFile2 (dataDir / "5.aol.data");
    mockFile2.close();

    stH.updateActiveFileID(dataDir);

    REQUIRE(stH.getActiveFileID() == 5);
}

TEST_CASE("KVStoreHandle sets a datafile extension for the store", "setDatafileExt()") {
    
    KVStoreHandle stH;
    stH.setDatafileExt(".data");

    REQUIRE(stH.getDatafileExt() == ".data");
}