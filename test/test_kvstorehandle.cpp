#include <assert.h>
#include <fstream>

#include "kvstorehandle.h"

void test_kvshandle_sets_absolute_directory() {

    KVStoreHandle stH;
    stH.setAbsDirPath();

    assert(std::filesystem::exists(stH.getAbsDirPath()));
}

void test_kvshandle_sets_activefile_id() {

    // create this folder in main for test.cpp and reuse for multiple assertions.

    KVStoreHandle stH;
    stH.setDatafileExt(".log");

    std::filesystem::path path = SOURCE_ROOT;
    std::filesystem::path dataDir = path / "test_data/";
    std::filesystem::create_directories(dataDir);

    std::ofstream mockFile1 (dataDir / "0.log");
    mockFile1.close();

    std::ofstream mockFile2 (dataDir / "5.log");
    mockFile2.close();

    stH.setActiveFileID(dataDir);

    assert(stH.getActiveFileID() == 5);

    std::filesystem::remove_all(dataDir);
}

void test_kvshandle_set_datafile_ext() {
    
    KVStoreHandle stH;
    stH.setDatafileExt(".data");

    assert(stH.getDatafileExt() == ".data");
}
int main() {

    test_kvshandle_sets_absolute_directory();
    test_kvshandle_sets_activefile_id();
    test_kvshandle_set_datafile_ext();
    return 0;
}