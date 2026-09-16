#include "kvstore.h"

#include <assert.h>

// void test_set_absolute_directory() {

//     KVStoreHandle handle;
//     handle.setAbsDirPath();

//     // myLSFS
//     // std::cout << handle.getAbsDirPath() << "\n";

//     assert(false);
// }

void test_open_store_sets_creates_data_dir() {

    KVStore st;

    std::optional<KVStoreHandle> handle = st.openStore(SOURCE_ROOT);
    assert(std::filesystem::exists(st.getDataDir()));

    return;
}

int main() {

    test_open_store_sets_creates_data_dir();

    return 0;
}