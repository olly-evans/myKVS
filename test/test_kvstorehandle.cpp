#include <assert.h>

#include "kvstorehandle.h"

void test_kvshandle_sets_absolute_directory() {

    KVStoreHandle handle;
    handle.setAbsDirPath();

    assert(std::filesystem::exists(handle.getAbsDirPath()));
}

void test_kvshandle_sets_activefile_id() {

    return;
}

int main() {
    test_kvshandle_sets_absolute_directory();
    test_kvshandle_sets_activefile_id();

    return 0;
}