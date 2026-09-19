#include "kvstorehandle.h"

/* HVStoreHandle Methods */

uint32_t KVStoreHandle::getActiveFileID() {
    return this->activeFileID;
}

void KVStoreHandle::setActiveFileID(std::filesystem::path dataDir) {

    uint32_t maxID = 0;
    bool found = false;

    for (const auto& datafile : std::filesystem::directory_iterator(dataDir)) {
        if (datafile.path().extension() == ".log") {
            uint32_t currentID = std::stoul(datafile.path().stem().string());
            maxID = std::max(maxID, currentID);
            found = true;
        }
    }

    this->activeFileID = found ? maxID : 0;  // 0 if this is a brand new store
}

std::filesystem::path KVStoreHandle::getAbsDirPath() {
    return this->absDirPath;
}

void KVStoreHandle::setAbsDirPath() {
    this->absDirPath = std::filesystem::path(SOURCE_ROOT);
}