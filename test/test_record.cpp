#include "record.h"

#include <assert.h>
#include <iostream>

void test_record_constructor() {

    std::string key = "k";
    std::string val = "v";
    Record rec(key, val);

    assert(rec.getTimestamp());
    
    assert(!rec.getKey().empty());
    assert(rec.getKeySize());

    assert(!rec.getValue().empty());
    assert(rec.getValueSize());

    std::cout << rec.getCRC32() << "\n";


}

int main() {

    test_record_constructor();
    return 0;
}