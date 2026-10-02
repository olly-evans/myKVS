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