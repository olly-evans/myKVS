#include "record.h"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Record constructor sets values, equal crc for same data", "Record()") {

    std::string key = "key";
    std::string val = "val";

    Record rec(key, val);               /* Constructor sets required fields. */
    REQUIRE(rec.getTimestamp());         /* Constructor should set timestamp to time now. */

    rec.setTimestamp(1000000);          /* Timestamp only changing variable, make it 
                                           constant to check crc consistent for same data. 
                                        */

    REQUIRE(rec.getTimestamp());
    
    REQUIRE(!rec.getKey().empty());      /* Constructor should set key. */
    REQUIRE(rec.getKeySize());           /* Constructor should set keySize. */

    REQUIRE(!rec.getValue().empty());    /* Constructor should set value. */
    REQUIRE(rec.getValueSize());         /* Constructor should set valueSize. */

    // Equal CRC using setCRC32() twice on same data.
    rec.setCRC32();
    uint32_t first = rec.getCRC32();

    rec.setCRC32();
    uint32_t second = rec.getCRC32();

    REQUIRE(first == second);            /* CRC should be equal for same data (const timestamp). */
}

TEST_CASE("Record with same bytes but differing values crc check is different", "Record()") {

    Record a("key", "val");
    Record b("key", "Xal");

    a.setTimestamp(100);
    b.setTimestamp(100);

    a.setCRC32();
    b.setCRC32();

    REQUIRE(a.getCRC32() != b.getCRC32());
}