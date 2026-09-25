#include "record.h"

#include <assert.h>

void test_record_constructor() {

    std::string key = "key";
    std::string val = "val";

    Record rec(key, val);               /* Constructor sets required fields. */
    assert(rec.getTimestamp());         /* Constructor should set timestamp to time now. */

    rec.setTimestamp(1000000);          /* Timestamp only changing variable, make it 
                                           constant to check crc consistent for same data. 
                                        */

    assert(rec.getTimestamp());
    
    assert(!rec.getKey().empty());      /* Constructor should set key. */
    assert(rec.getKeySize());           /* Constructor should set keySize. */

    assert(!rec.getValue().empty());    /* Constructor should set value. */
    assert(rec.getValueSize());         /* Constructor should set valueSize. */

    // Equal CRC using setCRC32() twice.
    rec.setCRC32();
    uint32_t first = rec.getCRC32();

    rec.setCRC32();
    uint32_t second = rec.getCRC32();

    assert(first == second);            /* CRC should be equal for same data (const timestamp). */
}

int main() {

    test_record_constructor();
    return 0;
}