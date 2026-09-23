#include "kvstorehandle.h"

/* HVStoreHandle Methods */

uint32_t KVStoreHandle::getActiveFileID() {
    return activeFileID;
}

void KVStoreHandle::setActiveFileID(std::filesystem::path dataDir) {

    uint32_t maxID = 0;
    bool found = false;

    for (const auto& datafile : std::filesystem::directory_iterator(dataDir)) {
        if (datafile.path().extension() == datafileExtension) {
            uint32_t currentID = std::stoul(datafile.path().stem().string());
            maxID = std::max(maxID, currentID);
            found = true;
        }
    }

    activeFileID = found ? maxID : 0;
}

std::filesystem::path KVStoreHandle::getAbsDirPath() {
    return absDirPath;
}

void KVStoreHandle::setAbsDirPath() {
    absDirPath = std::filesystem::path(SOURCE_ROOT);
}

std::string KVStoreHandle::getDatafileExt() {
    return datafileExtension;
}

void KVStoreHandle::setDatafileExt(std::string fileExtension) {
    datafileExtension = fileExtension;
}