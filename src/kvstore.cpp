#include <iostream>

#include "kvstore.h"

uint64_t KVStoreHandle::getActiveFileID() {
    return this->activeFileID;
}

void KVStoreHandle::setActiveFileID(uint64_t newID) {
    this->activeFileID = newID;
}

std::filesystem::path KVStoreHandle::getAbsDirPath() {
    return absDirPath;
}

void KVStoreHandle::setAbsDirPath() {
    absDirPath = std::filesystem::u8path(SOURCE_ROOT);
}

void KVStore::put(KVStoreHandle storeHandle, Record rec) {

    // Get the active datafile
    if (storeHandle.getActiveFileID() == -1)
        // initialise a new file to write to.

        // need root dir.
        // int id = open();
        // storeHandle.setActiveFileID(2);
    uint64_t id = storeHandle.getActiveFileID();
}