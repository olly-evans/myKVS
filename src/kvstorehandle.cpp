#include "kvstorehandle.h"

/* HVStoreHandle Methods */

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
    this->absDirPath = std::filesystem::path(SOURCE_ROOT);
}