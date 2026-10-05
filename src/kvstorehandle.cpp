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

uint32_t KVStoreHandle::validDatafileToID(fs::path path) {
    return std::stoul(path.stem().stem().string());
}

std::string KVStoreHandle::readString(std::ifstream& in, size_t n) {
    std::string s(n, '\0');
    if (!in.read(s.data(), static_cast<std::streamsize>(n)))
        throw std::runtime_error("short read");
    return s;
}

Record KVStoreHandle::readRecord(std::ifstream& in, const std::streamoff recFileOffset) {

    /* 
        Can take a path and in.open(), we will be using this for restore. 
        Only one run at init. 
    */

    /* size_t sometimes used as offset, hence cast. */
    if (recFileOffset > 0)
        in.seekg(static_cast<std::streamoff>(recFileOffset), std::ios_base::beg);

    uint32_t crc       = readField<uint32_t>(in);
    uint64_t timestamp = readField<uint64_t>(in);
    uint32_t keySize   = readField<uint32_t>(in);
    uint32_t valSize   = readField<uint32_t>(in);
    std::string key    = readString(in, keySize);
    std::string val    = readString(in, valSize);

    Record rec(key, val);
    rec.setTimestamp(timestamp);
    rec.setCRC32();

    if (rec.getCRC32() != crc)
        throw std::runtime_error("Read CRC and Calculated CRC not equal.");
        
    return rec;
}

void KVStoreHandle::updateKeyDir(fs::path path, const Record rec) {
    
    bool readPathIsActiveDatafile = fs::equivalent(path, activeDatafilePath);

    uint32_t readPathID = validDatafileToID(path);
    uint32_t id = readPathIsActiveDatafile ? getActiveDatafileID() : readPathID;

    size_t fileBytes = fs::file_size(path); // Put before this is used, file must have bytes.
    uint64_t valueByteOffset = fileBytes - rec.getValueSize();

    keyDir[rec.getKey()] = KeyDirEntry{id,
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
            uint32_t currentID = validDatafileToID(datafile);
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