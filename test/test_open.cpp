#include "kvstore.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("KVStore sets the max datafile bytes.", "KVStore::setMaxDatafileBytes()") {
    KVStore kvs;

    size_t bytes = 3;
    kvs.setMaxDatafileBytes(bytes);
    REQUIRE(kvs.getMaxDatafileBytes() == bytes);
}

TEST_CASE("KVStore sets the absolute directory", "KVStore::updateAbsDirPath") {

    KVStore kvs;
    kvs.updateRootDirPath();

    REQUIRE(fs::exists(kvs.getRootDirPath()));
}

TEST_CASE("On kvstore open are members set", "KVStore::open()") {

    KVStore kvs;
    StoreFlags flags;
    
    fs::path dataDir = createTempTestDir("test_open_1/");
    TempDirGuard cleanup{dataDir};

    KVStoreHandle stH = kvs.open(dataDir, flags);

    REQUIRE(fs::exists(kvs.getDataDir()));             /* Should have existing data directory. */
    REQUIRE(!stH.getDatafileExt().empty());            /* Datafile extension cannot be empty. */
    REQUIRE(kvs.getActiveOutputStream().good());       /* No error state in stream. */
    REQUIRE(kvs.getActiveOutputStream().is_open());    /* File should be open. */

    REQUIRE(kvs.getActiveInputStream().good());       /* No error state in input stream. */
    REQUIRE(kvs.getActiveInputStream().is_open());    /* Default datafile should be open to read. */

    REQUIRE(stH.getActiveDatafileID() == 0);           /* No puts so id should default to zero. */

    REQUIRE(stH.getActiveDatafilePath() == 
           dataDir / "0.aol.data");
}

TEST_CASE("Open gets the correct active id for a mock existing store", "KVStore::open()") {
    
    KVStore kvs;

    fs::path dataDir = createTempTestDir("test_open_1");
    TempDirGuard cleanup(dataDir);

    fs::path mFilePath1 = dataDir / "1.aol.data";
    std::ofstream mockFile1(mFilePath1);
    mockFile1.close();

    fs::path mFilePath2 = dataDir / "2.aol.data";
    std::ofstream mockFile2(mFilePath2);
    mockFile2.close();

    fs::path mFilePath3 = dataDir / "3.aol.data";
    std::ofstream mockFile3(mFilePath3);
    mockFile3.close();
    
    StoreFlags flags;

    KVStoreHandle stH = kvs.open(dataDir, flags);

    REQUIRE(fs::exists(kvs.getDataDir()));            /* Should have existing data directory. */
    REQUIRE(!stH.getDatafileExt().empty());           /* Datafile extension cannot be empty. */
    REQUIRE(kvs.getActiveOutputStream().good());      /* No error state in stream. */
    REQUIRE(kvs.getActiveOutputStream().is_open());   /* File should be open. */

    REQUIRE(kvs.getActiveInputStream().good());       /* No error state in input stream. */
    REQUIRE(kvs.getActiveInputStream().is_open());    /* Default datafile should be set to read. */

    REQUIRE(stH.getActiveDatafileID() == 3);           
    REQUIRE(stH.getActiveDatafilePath() == 
           dataDir / "3.aol.data");
}