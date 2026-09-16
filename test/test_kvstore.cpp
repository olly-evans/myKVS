#include "kvstore.h"

#include <assert.h>

// void test_set_absolute_directory() {

//     KVStoreHandle handle;
//     handle.setAbsDirPath();

//     // myLSFS
//     // std::cout << handle.getAbsDirPath() << "\n";

//     assert(false);
// }

void test_open_store() {

    KVStore kvs;

    KVStoreHandle h = kvs.openStore(SOURCE_ROOT);

    assert(h.getActiveFileID());
    assert(std::filesystem::exists(kvs.getDataDir()));

    return;
}

int main() {

    test_open_store();

    return 0;
}