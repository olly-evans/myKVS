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

    REQUIRE_FALSE(DetectStore::isNumericStem(example1));
    REQUIRE_FALSE(DetectStore::isNumericStem(example2));
    REQUIRE_FALSE(DetectStore::isNumericStem(example3));
    
    std::string example4 = "000001";

    REQUIRE(DetectStore::isNumericStem(example4));
}

TEST_CASE("isValidStoreFile() stripping filename correctly", "DetectStore::isValidStoreFile()") {
    
    TempDirGuard guard{TestHelpers::create_tmp_dir()};

    fs::path mFilePath1 = guard.dir / "0.aol.data";
    fs::path mFilePath2 = guard.dir / "0.data";

    REQUIRE(DetectStore::isValidStoreFile(mFilePath1));
    REQUIRE_FALSE(DetectStore::isValidStoreFile(mFilePath2));
}

TEST_CASE("Does data directory exist for isStore()", "DetectStore::isStore()") {
    TempDirGuard guard{TestHelpers::create_tmp_dir()};

    REQUIRE_FALSE(DetectStore::hasOnlyValidStoreFiles(guard.dir));
    REQUIRE_FALSE(DetectStore::isStore(guard.dir));

}

TEST_CASE("Does empty directory return false.", "DetectStore::isStore()") {
    TempDirGuard guard(TestHelpers::create_tmp_dir());

    REQUIRE_FALSE(DetectStore::isStore(guard.dir));

}
TEST_CASE("Detecting mock real store correctly", "DetectStore::isStore()") {

    TempDirGuard guard{TestHelpers::create_tmp_dir()};

    TestHelpers::create_mock_rol_file(guard.dir, ".hint", 1);
    TestHelpers::create_mock_rol_file(guard.dir, ".lock", 3);

    TestHelpers::create_mock_aol_file(guard.dir, ".data", 2);
   
    REQUIRE(DetectStore::isStore(guard.dir));

    TestHelpers::create_mock_rol_file(guard.dir, ".log", 4);
    
    REQUIRE_FALSE(DetectStore::isStore(guard.dir));
}

TEST_CASE("Detecting two append-only files in data directory", "DetectStore::isStore()") {

    TempDirGuard guard{TestHelpers::create_tmp_dir()};

    TestHelpers::create_mock_aol_file(guard.dir, ".data", 0);
    REQUIRE(DetectStore::isStore(guard.dir));

    TestHelpers::create_mock_aol_file(guard.dir, ".data", 1);
    REQUIRE_FALSE(DetectStore::isStore(guard.dir));
}
