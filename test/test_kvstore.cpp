#include "kvstore.h"

#include <assert.h>

void test_open_store() {

    KVStore kvs;
    StoreOptions storeOp;

    assert(!std::filesystem::exists(kvs.getDataDir()));

    KVStoreHandle h = kvs.openStore(SOURCE_ROOT, storeOp);

    assert(std::filesystem::exists(kvs.getDataDir()));

    assert(kvs.getActiveFilestream().good());
    assert(kvs.getActiveFilestream().is_open());

    return;
}

// Want to test with threads whether we can write to a locked dir.
// Simulate two processes.
void test_sync_on_put() {
    return;
}

int main() {
    test_open_store();

    return 0;
}