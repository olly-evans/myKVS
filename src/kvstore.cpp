#include "kvstore.h"

uint64_t KVStoreHandle::getActiveFileID() {
    return this->activeFileID;
}

void KVStoreHandle::setActiveFileID(uint64_t newID) {
    this->activeFileID = newID;
}

std::filesystem::path KVStoreHandle::getAbsDirPath() {
    return this->absDirPath;
}

void KVStoreHandle::setAbsDirPath() {
    this->absDirPath = std::filesystem::u8path(SOURCE_ROOT);
}

std::filesystem::path KVStore::getDataDir() {
    return this->dataDir;
}

void KVStore::setDataDir(std::filesystem::path root) {
    this->dataDir = root.append("/data/");
}

std::optional<KVStoreHandle> KVStore::openStore(std::filesystem::path dirPath) {

    KVStoreHandle stH; // Store handle.

    stH.setAbsDirPath();
    this->setDataDir(stH.getAbsDirPath());

    return stH;
    // if (!std::filesystem::exists(this->dataDir))

}

void KVStore::put(KVStoreHandle storeHandle, Record rec) {


    // data folder empty? or datafile dir empty? where is this stored. probably in KVStore.
    // if (!std::filesystem::exists(storeHandle.getAbsDirPath().append("/data/")))
    //     return;

    // lets assume our KVStoreHandle has been init properly with -1 by default.
    


    // Get the active datafile
    if (storeHandle.getActiveFileID() == -1ULL)
        // initialise a new file to write to.

        // need root dir.
        // int id = open();
        // storeHandle.setActiveFileID(2);
    uint64_t id = storeHandle.getActiveFileID();
}