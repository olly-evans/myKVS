#include "kvstorehandle.h"

/* HVStoreHandle Methods */

uint64_t KVStoreHandle::getActiveFileID() {
    return this->activeFileID;
}

void KVStoreHandle::setActiveFileID(std::filesystem::path dataDir) {
    // check our data dir for number
    uint64_t maxID = 0;
    bool found = false;

    for (const auto& datafile : std::filesystem::directory_iterator(dataDir)) {
        return;
    }
    // this->activeFileID = newID;
}

std::filesystem::path KVStoreHandle::getAbsDirPath() {
    return this->absDirPath;
}

void KVStoreHandle::setAbsDirPath() {
    this->absDirPath = std::filesystem::path(SOURCE_ROOT);
}