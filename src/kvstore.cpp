#include "kvstore.h"

namespace fs = std::filesystem;

/* KVStore Methods */

KVStoreHandle KVStore::open(const fs::path relDataDir, const StoreFlags stFlags) {

    // if stFlags.syncOnPut ... -> mutex in put function???
    // if stFlags.readWrite ... -> find way to allow read and write to dir.

    KVStoreHandle stH;

    stH.setAbsDirPath();
    setDataDir(stH.getAbsDirPath(), relDataDir);

    // Create dir if needed.
    if (!fs::exists(dataDir)) 
        fs::create_directories(dataDir);

    stH.setMaxDatafileBytes(stFlags.maxDatafileBytes);
    stH.setDatafileExt(stFlags.datafileExtension);

    if (stH.getDatafileExt().empty())
        stH.setDatafileExt(".data");

    // Get the active datafiles ID (highest filename) in dataDir.
    stH.updateActiveDatafileID(dataDir); 

    fs::path currentDatafilePath = createDatafilePath(stH.getActiveFileID(), stH.getDatafileExt()); 
    
    stH.setReadWrite(stFlags.readWrite); 
    setActiveDatafile(currentDatafilePath, stFlags.readWrite);

    std::cout << "Store opened successfully in:\n" << dataDir << std::endl;

    return stH;
}

void KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    std::unique_lock<std::shared_mutex> lock(writeMutex);

    if (!fs::exists(dataDir)) {
        std::cout << "[WARNING] You must open a store before using put." << std::endl;
        return;
    }
    
    if (!(fs::exists(activeDatafilePath) && fs::is_regular_file(activeDatafilePath))) {
        std::cout << "[WARNING] No active datafile path. Consider opening a store!" << std::endl;
        return;
    }

    std::string activePathID = activeDatafilePath.stem().stem();
    std::string activeHandleID = std::to_string(stH.getActiveFileID());

    if (activePathID != activeHandleID) {
        std::cerr << "[ERROR] Active datafile doesn't match the active ID." << std::endl;
        return;
    }

    if (!activeFilestream.is_open() || activeFilestream.bad())
        std::cerr << "[ERROR] Filestream error!" << std::endl;
    
    Record rec(key, val);

    if (fs::file_size(activeDatafilePath) + rec.byteSize() < stH.getMaxDatafileBytes()) {
        // If program crash occurs between serialize and entry, flush() occurs and we can load from disk.
        putRecord(stH, rec);
        return;
    } 

    // if its greater, make the new file and write. overlap is stupid and error prone.

    std::cout << "Datafile full, rolling-over..." << std::endl;

    rollOverDatafile(stH);
    stH.updateActiveDatafileID(dataDir);
    
    putRecord(stH, rec);    
}

std::optional<std::string> KVStore::get(const KVStoreHandle& stH, const std::string key) {

    /* Seperate filestream in get than put. */
    std::shared_lock<std::shared_mutex> lock(readMutex);

    auto it = stH.keyDir.find(key);
    if (it == stH.keyDir.end()) {
        return std::nullopt;
    }

    KeyDirEntry entry = it->second;

    /* Seperate filestreams opened for readCRC() and readValue(). */
    std::string valbuf = readValue(entry, stH.getDatafileExt());

    Record rec(key, valbuf); 
    size_t crcDisk = readCRC(entry, rec, stH.getDatafileExt());

    if (rec.getCRC32() != crcDisk)
        return std::nullopt; /* Recommend deleting key {key} */
    
    return valbuf;
}

std::vector<std::string> KVStore::listKeys(const KVStoreHandle& stH) {

    std::vector<std::string> keys;
    keys.reserve(stH.keyDir.size());

    for (const auto& entry : stH.keyDir) {
        keys.push_back(entry.first);
    }

    return keys;
}

void KVStore::putRecord(KVStoreHandle& stH, const Record rec) {
        
    rec.serialize(activeFilestream);

    size_t fileBytes = fs::file_size(activeDatafilePath);
    uint64_t valueByteOffset = fileBytes - rec.getValueSize();

    // This operator behaves funnily but forgot how.
    stH.keyDir[rec.getKey()] = KeyDirEntry{stH.getActiveFileID(),
                                       rec.getValueSize(),
                                       valueByteOffset,
                                       rec.getTimestamp()};
    
    std::cout << "Successfully serialized data to "   <<
                  activeDatafilePath.filename()       <<
                 "\nKeyDir entries is: "              << 
                  stH.keyDir.size()                   <<
                  std::endl;
}

/* File */

fs::path KVStore::createDatafilePath(uint32_t fileID, std::string fileExtension) const {
    
    std::string strID = std::to_string(fileID);
    fs::path appendOnlyDatafileExtension(strID + ".aol" + fileExtension);
    return dataDir / appendOnlyDatafileExtension;
}

void KVStore::setActiveDatafile(const fs::path path, const bool readWrite) {

    fs::perms rwPerms = fs::perms::owner_write | fs::perms::group_write | 
                        fs::perms::owner_read  | fs::perms::group_read;

    fs::perms roPerms = fs::perms::owner_read  | fs::perms::group_read;

    activeFilestream.open(path, std::ios::binary | std::ios::app);

    fs::perms fPermissions = readWrite ? rwPerms : roPerms;
    fs::permissions(path, fPermissions, fs::perm_options::replace);

    setActiveDatafilePath(path);

    return;
}

void KVStore::rollOverDatafile(const KVStoreHandle& stH) {

    makeDatafileReadOnly(activeDatafilePath);
    fs::path nextDatafilePath = createDatafilePath(stH.getActiveFileID() + 1, stH.getDatafileExt());
    setActiveDatafile(nextDatafilePath, stH.getReadWrite());

}

void KVStore::makeDatafileReadOnly(fs::path path) {

    activeFilestream.close();

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

/* Getters and Setters */

fs::path KVStore::getDataDir() const {
    return dataDir;
}

void KVStore::setDataDir(fs::path root, std::string dirName) {
    dataDir = root.append(dirName);
}

void KVStore::setActiveFilestream(std::ofstream stream) {
    activeFilestream = std::move(stream);
}

std::ofstream& KVStore::getActiveFilestream() {
    return activeFilestream;
}

void KVStore::setActiveDatafilePath(fs::path path) {
    activeDatafilePath = path;
}

fs::path KVStore::getActiveDatafilePath() const {
    return activeDatafilePath;
}

uint32_t KVStore::readCRC(KeyDirEntry entry, Record rec, std::string fileExtension) const {

    fs::path readPath = createDatafilePath(entry.fileID, fileExtension);

    std::ifstream in(readPath, std::ios::binary | std::ios::in);

    uint64_t crcOffset = (entry.valFileOffset + entry.valSz) - rec.byteSize();

    in.seekg(crcOffset, std::ios_base::beg);

    uint32_t crcFromDisk;
    in.read(reinterpret_cast<char*>(&crcFromDisk), sizeof(crcFromDisk));
    in.close();

    return crcFromDisk;
}

std::string KVStore::readValue(KeyDirEntry entry, std::string fileExtension) const {
    
    fs::path readPath = createDatafilePath(entry.fileID, fileExtension);
    std::ifstream in(readPath, std::ios::binary | std::ios::in);

    in.seekg(entry.valFileOffset, std::ios_base::beg);
    std::string val(entry.valSz, '\0');
    in.read(val.data(), entry.valSz);
    in.close();

    return val;
}