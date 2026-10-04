#include "kvstorehandle.h"

/* HVStoreHandle Methods */

fs::path KVStoreHandle::createDatafilePath(const fs::path& dataDir, uint32_t fID, std::string fExt) const {
    
    std::string strID = std::to_string(fID);
    fs::path appendOnlyName(strID + ".aol" + fExt);
    return dataDir / appendOnlyName;
}

void KVStoreHandle::setActiveDatafile(const fs::path& path, std::ofstream& out) {

    fs::perms rwPerms = fs::perms::owner_write | fs::perms::group_write | 
                        fs::perms::owner_read  | fs::perms::group_read;

    fs::perms roPerms = fs::perms::owner_read  | fs::perms::group_read;

    out.open(path, std::ios::binary | std::ios::app);

    fs::perms fPermissions = getReadWrite() ? rwPerms : roPerms;
    fs::permissions(path, fPermissions, fs::perm_options::replace);

    setActiveDatafilePath(path);

    return;
}

void KVStoreHandle::makeDatafileReadOnly(const fs::path& path, std::ofstream& out) {

    out.close();

    fs::path newPath = path;
    std::string filename = newPath.filename().string();
    filename.replace(filename.find("aol"), 3, "rol"); 
    newPath.replace_filename(filename);

    fs::rename(path, newPath);

    fs::permissions(newPath,
                    fs::perms::owner_read | fs::perms::group_read, 
                    fs::perm_options::replace);

    return;
}

void KVStoreHandle::rollOverDatafile(const fs::path& dataDir, std::ofstream& out) {

    makeDatafileReadOnly(activeDatafilePath, out);
    fs::path nextDatafilePath = createDatafilePath(dataDir, getActiveDatafileID() + 1, getDatafileExt());
    setActiveDatafile(nextDatafilePath, out);

}

uint32_t KVStoreHandle::readCRC(std::ifstream& in, size_t &crcOffset) {

    // file?

    // streampos?
    
    in.seekg(crcOffset, std::ios_base::beg);
    in.read(reinterpret_cast<char*>(&crcOffset), sizeof(crcOffset));

}

uint32_t KVStoreHandle::readDiskCRC(fs::path path, KeyDirEntry entry, Record rec) const {

    std::ifstream in(path, std::ios::binary | std::ios::in);

    uint64_t crcOffset = (entry.valFileOffset + entry.valSz) - rec.byteSize();

    in.seekg(crcOffset, std::ios_base::beg);

    uint32_t crcFromDisk;
    in.read(reinterpret_cast<char*>(&crcFromDisk), sizeof(crcFromDisk));
    in.close();

    return crcFromDisk;
}

std::string KVStoreHandle::readDiskValue(fs::path path, KeyDirEntry entry) const {
    
    std::ifstream in(path, std::ios::binary | std::ios::in);

    in.seekg(entry.valFileOffset, std::ios_base::beg);
    std::string val(entry.valSz, '\0');
    in.read(val.data(), entry.valSz);
    in.close();

    return val;
}

void KVStoreHandle::updateKeyDir(fs::path path, const Record rec) {
        
    size_t fileBytes = fs::file_size(path);
    uint64_t valueByteOffset = fileBytes - rec.getValueSize();

    keyDir[rec.getKey()] = KeyDirEntry{getActiveDatafileID(),
                                       rec.getValueSize(),
                                       valueByteOffset,
                                       rec.getTimestamp()};
}

void KVStoreHandle::setActiveDatafilePath(fs::path path) {
    activeDatafilePath = path;
}

fs::path KVStoreHandle::getActiveDatafilePath() const {
    return activeDatafilePath;
}

uint32_t KVStoreHandle::getActiveDatafileID() const {
    return activeDatafileID;
}

void KVStoreHandle::updateActiveDatafileID(fs::path dataDir) {

    uint32_t maxID = 0;
    bool found = false;

    for (const auto& datafile : fs::directory_iterator(dataDir)) {
        
        if (datafile.path().extension() == datafileExtension) {
            uint32_t currentID = std::stoul(datafile.path().stem().stem().string());
            maxID = std::max(maxID, currentID);
            found = true;
        }
    }
    activeDatafileID = found ? maxID : 0;
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