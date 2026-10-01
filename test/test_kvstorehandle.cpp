#include <assert.h>
#include <fstream>

#include "kvstorehandle.h"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("KVStoreHandle sets the absolute directory") {

    KVStoreHandle stH;
    stH.setAbsDirPath();

    REQUIRE(std::filesystem::exists(stH.getAbsDirPath()));
}

TEST_CASE("KVStoreHandle updates the active datafile id in datadir", "updateActiveFileID()") {

    KVStoreHandle stH;

    std::filesystem::path path = SOURCE_ROOT;
    std::filesystem::path dataDir = path / "test_data/";
    std::filesystem::create_directories(dataDir);

    std::ofstream mockFile1 (dataDir / "0.aol.data");
    mockFile1.close();

    std::ofstream mockFile2 (dataDir / "5.aol.data");
    mockFile2.close();

    stH.updateActiveFileID(dataDir);

    REQUIRE(stH.getActiveFileID() == 5);

    std::filesystem::remove_all(dataDir);
}

TEST_CASE("KVStoreHandle sets a datafile extension for the store", "setDatafileExt()") {
    
    KVStoreHandle stH;
    stH.setDatafileExt(".data");

    REQUIRE(stH.getDatafileExt() == ".data");
}