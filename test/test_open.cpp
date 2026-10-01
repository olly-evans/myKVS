#include "kvstore.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>


TEST_CASE("On kvstore open are members set", "KVStore::open()") {

    KVStore kvs;
    StoreFlags flags;

    flags.datafileExtension = ".data";
    KVStoreHandle stH = kvs.open(dir, flags);

    REQUIRE(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    REQUIRE(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    REQUIRE(kvs.getActiveFilestream().good());       /* No error state in stream. */
    REQUIRE(kvs.getActiveFilestream().is_open());    /* File should be open. */

    REQUIRE(stH.getActiveFileID() == 0);             /* No puts so id should default to zero. */

    REQUIRE(kvs.getActiveDatafilePath() == 
           dir / "0.aol.data");
}

TEST_CASE("Mock existing store gets the correct active file id", "KVStore::") {
    
    KVStore kvs;

    fs::path mFilePath1 = dir / "1.aol.log";
    std::ofstream mockFile1(mFilePath1);
    mockFile1.close();

    fs::path mFilePath2 = dir / "2.aol.log";
    std::ofstream mockFile2(mFilePath2);
    mockFile2.close();

    fs::path mFilePath3 = dir / "3.aol.log";
    std::ofstream mockFile3(mFilePath3);
    mockFile3.close();
    
    StoreFlags flags;
    flags.datafileExtension = ".log";
    KVStoreHandle stH = kvs.open(dir, flags);

    REQUIRE(fs::exists(kvs.getDataDir()));           /* Should have existing data directory. */
    REQUIRE(!stH.getDatafileExt().empty());          /* Datafile extension cannot be empty. */
    REQUIRE(kvs.getActiveFilestream().good());       /* No error state in stream. */
    REQUIRE(kvs.getActiveFilestream().is_open());    /* File should be open. */

    REQUIRE(stH.getActiveFileID() == 3);           
    REQUIRE(kvs.getActiveDatafilePath() == 
           dir / "3.aol.log");
    
    
    fs::remove(mFilePath1);
    fs::remove(mFilePath2);
    fs::remove(mFilePath3);
}