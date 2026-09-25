#include "record.h"

#include <assert.h>
#include <iostream>

void test_record_constructor() {

    std::string key = "key";
    std::string val = "val";

    Record rec(key, val); // Constructor sets required fields.
    
    assert(rec.getTimestamp());
    rec.setTimestamp(1000000); // Overwrite timestamp set in constructor for testing.


    assert(rec.getTimestamp());
    
    assert(!rec.getKey().empty());
    assert(rec.getKeySize());

    assert(!rec.getValue().empty());
    assert(rec.getValueSize());

    // Equal CRC using setCRC32() twice.
    rec.setCRC32();
    uint32_t first = rec.getCRC32();
    rec.setCRC32();
    uint32_t second = rec.getCRC32();
    assert(first == second);

    std::cout << rec.getCRC32() << "\n";


}

int main() {

    test_record_constructor();
    return 0;
}