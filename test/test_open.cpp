#include "kvstore.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("KVStore sets the max datafile bytes.", "KVStoreHandle::setMaxDatafileBytes()") {
    KVStore kvs;

    size_t bytes = 3;
    kvs.setMaxDatafileBytes(bytes);
    REQUIRE(kvs.getMaxDatafileBytes() == bytes);
}

TEST_CASE("On kvstore open are members set", "KVStore::open()") {

    KVStore kvs;
    StoreFlags flags;
    
    fs::path dataDir = createTempTestDir("test_open_1/");
    TempDirGuard cleanup{dataDir};

    KVStoreHandle stH = kvs.open(dataDir, flags);

    REQUIRE(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    REQUIRE(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    REQUIRE(kvs.getActiveFilestream().good());       /* No error state in stream. */
    REQUIRE(kvs.getActiveFilestream().is_open());    /* File should be open. */

    REQUIRE(stH.getActiveFileID() == 0);             /* No puts so id should default to zero. */

    REQUIRE(kvs.getActiveDatafilePath() == 
           dataDir / "0.aol.data");
}

TEST_CASE("Mock existing store gets the correct active file id", "KVStore::") {
    
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

    REQUIRE(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    REQUIRE(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    REQUIRE(kvs.getActiveFilestream().good());       /* No error state in stream. */
    REQUIRE(kvs.getActiveFilestream().is_open());    /* File should be open. */

    REQUIRE(stH.getActiveFileID() == 3);           
    REQUIRE(kvs.getActiveDatafilePath() == 
           dataDir / "3.aol.data");
}