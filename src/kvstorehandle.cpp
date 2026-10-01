#include "kvstorehandle.h"

/* HVStoreHandle Methods */

uint32_t KVStoreHandle::getActiveFileID() const {
    return activeFileID;
}

void KVStoreHandle::updateActiveFileID(std::filesystem::path dataDir) {

    std::cout << dataDir << "\n";
    uint32_t maxID = 0;
    bool found = false;

    for (const auto& datafile : std::filesystem::directory_iterator(dataDir)) {
        
        if (datafile.path().extension() == datafileExtension) {
            uint32_t currentID = std::stoul(datafile.path().stem().stem().string());
            maxID = std::max(maxID, currentID);
            found = true;
        }
    }
    activeFileID = found ? maxID : 0;
}

size_t KVStoreHandle::getMaxDatafileBytes() const {
    return maxDatafileBytes;
}

void KVStoreHandle::setMaxDatafileBytes(size_t maxBytes) {
    maxDatafileBytes = maxBytes;
}

std::filesystem::path KVStoreHandle::getAbsDirPath() const {
    return absDirPath;
}

void KVStoreHandle::setAbsDirPath() {
    absDirPath = std::filesystem::path(SOURCE_ROOT);
}

std::string KVStoreHandle::getDatafileExt() const {
    return datafileExtension;
}

void KVStoreHandle::setDatafileExt(std::string fileExtension) {
    datafileExtension = fileExtension;
}

bool KVStoreHandle::getReadWrite() const {
    return readWrite;
}

void KVStoreHandle::setReadWrite(bool rw) {
    readWrite = rw;
}