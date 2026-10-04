
#include "detect_store.h"
#include "kvstore.h"

namespace fs = std::filesystem;

/* KVStore Methods */

KVStoreHandle KVStore::open(const fs::path relDataDir, const StoreFlags stFlags) {

    // if stFlags.syncOnPut ... -> mutex in put function???
    // if stFlags.readWrite ... -> find way to allow read and write to dir.

    KVStoreHandle stH;

    stH.setAbsDirPath();
    setDataDir(stH.getAbsDirPath(), relDataDir);    

    setMaxDatafileBytes(stFlags.maxDatafileBytes);

    stH.setDatafileExt(".data");

    stH.setReadWrite(stFlags.readWrite); 

    /* isDataDir doing too much. */
    if (fs::exists(dataDir) && DetectStore::isStore(dataDir)) {
        restore(stH);

        stH.updateActiveDatafileID(dataDir);
        fs::path currentDatafilePath = createDatafilePath(stH.getActiveDatafileID(), stH.getDatafileExt()); 
        
        setActiveDatafile(currentDatafilePath, stFlags.readWrite);

        std::cout << "Existing store opened successfully in:\n << dataDir" << std::endl;
        return stH;
    }

    fs::create_directories(dataDir);

    // we can make a setter for this for this case.
    stH.updateActiveDatafileID(dataDir);
    fs::path currentDatafilePath = createDatafilePath(stH.getActiveDatafileID(), stH.getDatafileExt()); 
    
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
    std::string activeHandleID = std::to_string(stH.getActiveDatafileID());

    if (activePathID != activeHandleID) {
        std::cerr << "[ERROR] Active datafile doesn't match the active ID." << std::endl;
        return;
    }

    if (!activeFilestream.is_open() || activeFilestream.bad())
        std::cerr << "[ERROR] Filestream error!" << std::endl;
    
    Record rec(key, val);

    if (fs::file_size(activeDatafilePath) + rec.byteSize() < getMaxDatafileBytes()) {
        // If program crash occurs between serialize and entry, flush() occurs and we can load from disk.
        rec.serialize(activeFilestream);
        stH.updateKeyDir(activeDatafilePath, rec);
        return;
    } 

    std::cout << "Datafile full, rolling-over..." << std::endl;

    rollOverDatafile(stH);
    stH.updateActiveDatafileID(dataDir);
    
    rec.serialize(activeFilestream);
    stH.updateKeyDir(activeDatafilePath, rec);
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

    fs::path datafile = createDatafilePath(entry.fileID, stH.getDatafileExt());

    std::string valbuf = stH.readDiskValue(datafile, entry);

    Record rec(key, valbuf); 
    size_t crcDisk = stH.readDiskCRC(datafile, entry, rec);

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

void KVStore::restore(KVStoreHandle& stH) {

        
    for (const auto& df : fs::directory_iterator(dataDir)) {

        // restoreRecord()
        uintmax_t bytesRead = 0;
        uintmax_t fileSize = df.file_size();

        std::ifstream in(df.path(), std::ios::binary | std::ios::in);

        while(bytesRead < fileSize) {
            
            // Record rec();
            // rec.deserialize(), deserializes and constructs record for us.

            size_t keySizeOffset = sizeof(uint32_t) + sizeof(uint64_t) + bytesRead;

            uint32_t keySize;
            uint32_t valSize;

            in.seekg(keySizeOffset, std::ios_base::beg);
            in.read(reinterpret_cast<char*>(&keySize), sizeof(uint32_t)); // read 4 bytes into buf from keySizeOffset.
            in.read(reinterpret_cast<char*>(&valSize), sizeof(uint32_t)); // read 4 bytes more should be valsize.
            
            bytesRead += keySizeOffset + sizeof(uint32_t) + sizeof(uint32_t) + keySize + valSize;

            std::string key(keySize, '\0');
            std::string val(valSize, '\0');


            in.read(key.data(), keySize);
            in.read(val.data(), valSize);

            Record rec(key, val);
            // rec.setTimestamp();
            
            if (rec.byteSize() != bytesRead)
                return;

            stH.updateKeyDir(df.path(), rec);
        }
    }
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
    fs::path nextDatafilePath = createDatafilePath(stH.getActiveDatafileID() + 1, stH.getDatafileExt());
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

size_t KVStore::getMaxDatafileBytes() const {
    return maxDatafileBytes;
}

void KVStore::setMaxDatafileBytes(size_t maxBytes) {
    maxDatafileBytes = maxBytes;
}