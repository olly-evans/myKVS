#include "detect_store.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

// perhaps a function to check two periods in str.

// TEST_CASE("Does isAppendOnlyDatafile detect status in a datafile", "DetectStore::isAppendOnlyDatafile()") {
//     fs::path mockDatafile = test_create_append_only_datafile_path("test_detect_store_aol", ".data", 0);
//     REQUIRE(DetectStore::isAppendOnlyDatafile(mockDatafile));
// }

TEST_CASE("Does isNumericStem() detect a numeric stem in range", "DetectStore::isNumericStem()") {

    uint64_t num = 64;
    for (uint64_t i = 0; i < num; i++) {
        REQUIRE(DetectStore::isNumericStem(std::to_string(i)));
    }
}

TEST_CASE("Does isNumericStem() detect a non-numeric stem", "DetectStore::isNumericStem()") {
    
    std::string example1 = "0.aol.data";
    std::string example2 = ".aol.data";
    std::string example3 = ".data";

    REQUIRE(!DetectStore::isNumericStem(example1));
    REQUIRE(!DetectStore::isNumericStem(example2));
    REQUIRE(!DetectStore::isNumericStem(example3));
    
    std::string example4 = "000001";

    REQUIRE(DetectStore::isNumericStem(example4));
}

TEST_CASE("isValidStoreFile() stripping filename correctly", "DetectStore::isValidStoreFile()") {
    
    fs::path dataDir = TestHelpers::create_tmp_dir("test_detect_store_1");
    TempDirGuard cleanup{dataDir};

    fs::path mFilePath1 = dataDir / "0.aol.data";
    fs::path mFilePath2 = dataDir / "0.data";

    REQUIRE(DetectStore::isValidStoreFile(mFilePath1));
    REQUIRE(!DetectStore::isValidStoreFile(mFilePath2));
}

TEST_CASE("Does data directory exist for isStore()", "KVStore::isStore()") {
    fs::path dataDir = TestHelpers::create_tmp_dir("test_detect_store_2");
    TempDirGuard cleanup{dataDir};

    REQUIRE(!DetectStore::hasOnlyValidStoreFiles(dataDir));
    REQUIRE(!DetectStore::isStore(dataDir));

}
TEST_CASE("Detecting mock real store correctly", "KVStore::isStore()") {

    fs::path dataDir = TestHelpers::create_tmp_dir("test_detect_store_3");
    TempDirGuard cleanup{dataDir};

    TestHelpers::create_mock_rol_file(dataDir, ".hint", 1);
    TestHelpers::create_mock_rol_file(dataDir, ".lock", 3);

    TestHelpers::create_mock_aol_file(dataDir, ".data", 2);
   
    REQUIRE(DetectStore::isStore(dataDir));

    TestHelpers::create_mock_rol_file(dataDir, ".log", 4);
    
    REQUIRE(!DetectStore::isStore(dataDir));
}

TEST_CASE("Detecting two append-only files in data directory", "KVStore::isStore()") {

    fs::path dataDir = TestHelpers::create_tmp_dir("test_detect_store_4");
    TempDirGuard cleanup{dataDir};

    TestHelpers::create_mock_aol_file(dataDir, ".data", 0);
    REQUIRE(DetectStore::isStore(dataDir));

    TestHelpers::create_mock_aol_file(dataDir, ".data", 1);
    REQUIRE(!DetectStore::isStore(dataDir));
}
