#include <assert.h>
#include <fstream>
#include <iostream>

#include "kvstorehandle.h"

void test_kvshandle_sets_absolute_directory() {

    KVStoreHandle stH;
    stH.setAbsDirPath();

    assert(std::filesystem::exists(stH.getAbsDirPath()));
}

void test_kvshandle_sets_activefile_id() {

    KVStoreHandle stH;

    std::filesystem::path path = SOURCE_ROOT;
    std::filesystem::path dataDir = path / "test_data";
    
    // make dummy files.
    std::ofstream mockFile1 (dataDir / "0.log");
    mockFile1.close();

    std::ofstream mockFile2 (dataDir / "5.log");
    mockFile2.close();

    stH.setActiveFileID(dataDir);

    assert(stH.getActiveFileID() == 5);

    return;
}

int main() {

    test_kvshandle_sets_absolute_directory();
    test_kvshandle_sets_activefile_id();

    return 0;
}