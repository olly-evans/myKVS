#include <assert.h>
#include <fstream>

#include "kvstorehandle.h"

void test_kvshandle_sets_absolute_directory() {

    KVStoreHandle stH;
    stH.setAbsDirPath();

    assert(std::filesystem::exists(stH.getAbsDirPath()));
}

void test_kvshandle_sets_activefile_id() {

    KVStoreHandle stH;

    std::filesystem::path path = SOURCE_ROOT;
    path.append("data/");
    
    // make dummy files.
    std::ofstream mockFile1 (path.append("0.log"));
    std::ofstream mockFile2 (path.append("5.log"));

    stH.setActiveFileID(path);

    assert(stH.getActiveFileID() == 5);

    mockFile1.close();
    mockFile2.close();

    return;
}

int main() {

    test_kvshandle_sets_absolute_directory();
    test_kvshandle_sets_activefile_id();

    return 0;
}