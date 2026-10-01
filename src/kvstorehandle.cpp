#include "kvstorehandle.h"

/* HVStoreHandle Methods */

uint32_t KVStoreHandle::readCRC(fs::path path, KeyDirEntry entry, Record rec) const {


    std::ifstream in(path, std::ios::binary | std::ios::in);

    uint64_t crcOffset = (entry.valFileOffset + entry.valSz) - rec.byteSize();

    in.seekg(crcOffset, std::ios_base::beg);

    uint32_t crcFromDisk;
    in.read(reinterpret_cast<char*>(&crcFromDisk), sizeof(crcFromDisk));
    in.close();

    return crcFromDisk;
}

std::string KVStoreHandle::readValue(fs::path path, KeyDirEntry entry) const {
    
    std::ifstream in(path, std::ios::binary | std::ios::in);

    in.seekg(entry.valFileOffset, std::ios_base::beg);
    std::string val(entry.valSz, '\0');
    in.read(val.data(), entry.valSz);
    in.close();

    return val;
}

void KVStoreHandle::writeRecord(fs::path path, std::ofstream& out, const Record rec) {
        
    rec.serialize(out);

    size_t fileBytes = fs::file_size(path);
    uint64_t valueByteOffset = fileBytes - rec.getValueSize();

    keyDir[rec.getKey()] = KeyDirEntry{getActiveFileID(),
                                       rec.getValueSize(),
                                       valueByteOffset,
                                       rec.getTimestamp()};
}

uint32_t KVStoreHandle::getActiveFileID() const {
    return activeFileID;
}

void KVStoreHandle::updateActiveDatafileID(std::filesystem::path dataDir) {

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
    absDirPath = std::filesystem::path(WORKING_DIRECTORY);
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