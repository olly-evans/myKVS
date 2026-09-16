#include <assert.h>

#include "kvstorehandle.h"

void test_kvshandle_sets_absolute_directory() {

    KVStoreHandle handle;
    handle.setAbsDirPath();

    assert(std::filesystem::exists(handle.getAbsDirPath()));
}

int main() {
    test_kvshandle_sets_absolute_directory();
    return 0;
}