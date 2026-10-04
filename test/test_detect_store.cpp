#include "detect_store.h"
#include "testing_helpers.h"

#include <catch2/catch_test_macros.hpp>

// perhaps a function to check two periods in str.

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
    
    fs::path dataDir = createTempTestDir("test_detect_store_1");
    TempDirGuard cleanup{dataDir};

    fs::path mFilePath1 = dataDir / "0.aol.data";
    fs::path mFilePath2 = dataDir / "0.data";

    REQUIRE(DetectStore::isValidStoreFile(mFilePath1));
    REQUIRE(!DetectStore::isValidStoreFile(mFilePath2));
}

TEST_CASE("Does data directory exist for isStore()", "KVStore::isStore()") {
    fs::path dataDir = createTempTestDir("test_detect_store_2");
    TempDirGuard cleanup{dataDir};

    REQUIRE(!DetectStore::hasOnlyValidStoreFiles(dataDir));
    REQUIRE(!DetectStore::isStore(dataDir));

}
TEST_CASE("Detecting mock real store correctly", "KVStore::isStore()") {

    fs::path dataDir = createTempTestDir("test_detect_store_3");
    TempDirGuard cleanup{dataDir};

    fs::path mFilePath1 = dataDir / "1.aol.hint";
    std::ofstream mockFile1(mFilePath1);
    mockFile1.close();

    fs::path mFilePath2 = dataDir / "2.aol.data";
    std::ofstream mockFile2(mFilePath2);
    mockFile2.close();

    fs::path mFilePath3 = dataDir / "3.aol.lock";
    std::ofstream mockFile3(mFilePath3);
    mockFile3.close();    

    REQUIRE(DetectStore::isStore(dataDir));

    fs::path mFilePath4 = dataDir / "4.aol.log"; /* Not a valid store file extension. */
    std::ofstream mockFile4(mFilePath4);
    mockFile4.close(); 
    
    REQUIRE(!DetectStore::isStore(dataDir));
}
