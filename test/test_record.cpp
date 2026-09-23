#include "record.h"

#include <assert.h>

void test_record_constructor() {

    Record rec("key", "value");

    assert(rec.getTimestamp());
    
    assert(!rec.getKey().empty());
    assert(rec.getKeySize());

    assert(!rec.getValue().empty());
    assert(rec.getValueSize());


}

int main() {

    
    return 0;
}