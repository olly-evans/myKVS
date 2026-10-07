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

TEST_CASE("Open rejects a non-absolute directory argument.", "KVStore::open()") {
    fs::path mock("not/absolute/");

    KVStore kvs;
    StoreFlags flags;
    KVExpected result = kvs.open(mock, flags);
    REQUIRE(result.error().code == KVErrorCode::ProvidedPathNotAbsolute);

}

TEST_CASE("On kvstore open are members set", "KVStore::open()") {

    KVStore kvs;
    StoreFlags flags;
    
    fs::path dataDir = TestHelpers::create_tmp_dir("test_open_1/");
    TempDirGuard cleanup{dataDir};

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

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

    fs::path dataDir = TestHelpers::create_tmp_dir("test_open_1");
    TempDirGuard cleanup(dataDir);

    TestHelpers::create_n_mock_files(dataDir, ".data", 1, 3);
    
    StoreFlags flags;

    KVExpected result = kvs.open(dataDir, flags);
    REQUIRE(result);
    KVStoreHandle& stH = result.value();

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