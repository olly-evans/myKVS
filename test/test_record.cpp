#include "record.h"

#include <assert.h>




int main() {

    Record rec("key", "value");

    assert(!rec.getKey().empty());
    assert(rec.getKeySize());

    assert(!rec.getValue().empty());
    assert(rec.getValueSize());
    
    return 0;
}