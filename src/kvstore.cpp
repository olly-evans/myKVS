#include <fstream>

#include "kvstore.h"

/* KVStore Methods */

std::filesystem::path KVStore::getDataDir() {
    return this->dataDir;
}

void KVStore::setDataDir(std::filesystem::path root) {
    this->dataDir = root.append("data/");
}

KVStoreHandle KVStore::openStore(std::filesystem::path dirPath) {

    KVStoreHandle stH; // Store handle.

    // int fd = open(new datafile);

    


    stH.setAbsDirPath();
    this->setDataDir(stH.getAbsDirPath());

    std::ofstream datafile ("test.txt");
    datafile.close();

    stH.setActiveFileID(0);

    return stH;
    // if (!std::filesystem::exists(this->dataDir))

}

void KVStore::put(KVStoreHandle stH, Record rec) {


    // data folder empty? or datafile dir empty? where is this stored. probably in KVStore.
    // if (!std::filesystem::exists(storeHandle.getAbsDirPath().append("/data/")))
    //     return;

    // lets assume our KVStoreHandle has been init properly with -1 by default.
    


    // Get the active datafile
    if (stH.getActiveFileID() == -1ULL)
        // initialise a new file to write to.

        // need root dir.
        // int id = open();
        // storeHandle.setActiveFileID(2);
    uint64_t id = stH.getActiveFileID();
}