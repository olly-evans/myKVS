#include "kvstore.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("On kvstore open are members set", "KVStore::open()") {

    KVStore kvs;
    StoreFlags flags;
    
    fs::path dataDir = createTempTestDir("test_open_1/");
    TempDirGuard cleanup{dataDir};

    flags.datafileExtension = ".data";
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

    fs::path dataDir = createTempTestDir("test_open_2");
    TempDirGuard cleanup(dataDir);

    fs::path mFilePath1 = dataDir / "1.aol.log";
    std::ofstream mockFile1(mFilePath1);
    mockFile1.close();

    fs::path mFilePath2 = dataDir / "2.aol.log";
    std::ofstream mockFile2(mFilePath2);
    mockFile2.close();

    fs::path mFilePath3 = dataDir / "3.aol.log";
    std::ofstream mockFile3(mFilePath3);
    mockFile3.close();
    
    StoreFlags flags;
    flags.datafileExtension = ".log";
    KVStoreHandle stH = kvs.open(dataDir, flags);

    REQUIRE(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    REQUIRE(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    REQUIRE(kvs.getActiveFilestream().good());       /* No error state in stream. */
    REQUIRE(kvs.getActiveFilestream().is_open());    /* File should be open. */

    REQUIRE(stH.getActiveFileID() == 3);           
    REQUIRE(kvs.getActiveDatafilePath() == 
           dataDir / "3.aol.log");
}