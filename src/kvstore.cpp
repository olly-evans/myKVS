
#include "detect_store.h"
#include "kvstore.h"

namespace fs = std::filesystem;

/* KVStore Methods */

KVStoreHandle KVStore::open(const fs::path relDataDir, const StoreFlags stFlags) {

    // if stFlags.syncOnPut ... -> mutex in put function???
    // if stFlags.readWrite ... -> find way to allow read and write to dir.

    KVStoreHandle stH;

    updateAbsDirPath();
    setDataDir(getAbsDirPath(), relDataDir);    

    setMaxDatafileBytes(stFlags.maxDatafileBytes);

    stH.setDatafileExt(".data");

    stH.setReadWrite(stFlags.readWrite); 

    /* isDataDir doing too much. */
    if (fs::exists(dataDir) && DetectStore::isStore(dataDir)) {

        restore(stH);

        stH.updateActiveDatafileID(dataDir);
        fs::path currentDatafilePath = stH.createDatafilePath(dataDir, 
                                                              stH.getActiveDatafileID(), 
                                                              stH.getDatafileExt()); 
        
        stH.setActiveDatafile(currentDatafilePath, activeOutputStream);
        
        /* Set to active datafile by default. */

        std::ifstream activeInputStream(currentDatafilePath, std::ios::binary | std::ios::in);
        setActiveInputStreamPath(currentDatafilePath);

        std::cout << "Existing store opened successfully in:\n << dataDir" << std::endl;
        return stH;
    }

    fs::create_directories(dataDir);

    // we can make a setter for this for this case.
    stH.updateActiveDatafileID(dataDir);
    fs::path currentDatafilePath = stH.createDatafilePath(dataDir, 
                                                          stH.getActiveDatafileID(), 
                                                          stH.getDatafileExt()); 
    
    stH.setActiveDatafile(currentDatafilePath, activeOutputStream);
    
    /* Set to active datafile by default. */

    std::ifstream activeInputStream(currentDatafilePath, std::ios::binary | std::ios::in);
    setActiveInputStreamPath(currentDatafilePath);

    std::cout << "Store opened successfully in:\n" << dataDir << std::endl;

    return stH;
}

void KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    std::unique_lock<std::shared_mutex> lock(writeMutex);

    if (!fs::exists(dataDir)) {
        std::cout << "[WARNING] You must open a store before using put." << std::endl;
        return;
    }
    
    fs::path df = stH.getActiveDatafilePath();

    if (!(fs::exists(df) && fs::is_regular_file(df))) {
        std::cout << "[WARNING] No active datafile path. Consider opening a store!" << std::endl;
        return;
    }

    std::string activePathID = df.stem().stem();
    std::string activeHandleID = std::to_string(stH.getActiveDatafileID());

    if (activePathID != activeHandleID) {
        std::cerr << "[ERROR] Active datafile doesn't match the active ID." << std::endl;
        return;
    }

    if (!activeOutputStream.is_open() || activeOutputStream.bad())
        std::cerr << "[ERROR] Filestream error!" << std::endl;
    
    Record rec(key, val);

    if (fs::file_size(df) + rec.byteSize() < getMaxDatafileBytes()) {
        // If program crash occurs between serialize and entry, flush() occurs and we can load from disk.
        rec.serialize(activeOutputStream);
        stH.updateKeyDir(df, rec);
        return;
    } 

    std::cout << "Datafile full, rolling-over..." << std::endl;

    stH.rollOverDatafile(dataDir, activeOutputStream);
    stH.updateActiveDatafileID(dataDir);
    
    rec.serialize(activeOutputStream);
    stH.updateKeyDir(stH.getActiveDatafilePath(), rec);
}

std::optional<std::string> KVStore::get(const KVStoreHandle& stH, const std::string key) {

    /* Seperate filestream in get than put. */
    std::shared_lock<std::shared_mutex> lock(readMutex);

    auto it = stH.keyDir.find(key);
    if (it == stH.keyDir.end()) {
        return std::nullopt;
    }

    KeyDirEntry entry = it->second;

    fs::path readPath = stH.createDatafilePath(dataDir, entry.fileID, stH.getDatafileExt());

    /* if input stream already reading from stH.getActiveDatafilePath(), no set */
    if (getActiveInputStreamPath() != readPath)
        updateActiveInputStream(readPath);
    

    // pass in stream perhaps, dont know if ill keep these.
    std::string valbuf = stH.readDiskValue(readPath, entry);

    Record rec(key, valbuf); 
    size_t crcDisk = stH.readDiskCRC(readPath, entry, rec);

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
            
            // readDiskCRC(std::ifstream& in, &bytesRead) <- incremented in readDiskCRC.

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

/* Getters and Setters */

fs::path KVStore::getAbsDirPath() const {
    return absDirPath;
}

void KVStore::updateAbsDirPath() {
    absDirPath = fs::path(WORKING_DIRECTORY);
}

fs::path KVStore::getDataDir() const {
    return dataDir;
}

void KVStore::setDataDir(fs::path root, std::string dirName) {
    dataDir = root.append(dirName);
}

std::ofstream& KVStore::getActiveOutputStream() {
    return activeOutputStream;
}

void KVStore::setActiveOutputStream(std::ofstream stream) {
    activeOutputStream = std::move(stream);
}

std::ifstream& KVStore::getActiveInputStream() {
    return activeInputStream;
}

void KVStore::updateActiveInputStream(fs::path path) {
    activeInputStream.open(path, std::ios::binary | std::ios::in);
    setActiveInputStreamPath(path);
}

fs::path KVStore::getActiveInputStreamPath() {
    return activeInputStreamPath;
}

void KVStore::setActiveInputStreamPath(fs::path path) {
    activeInputStreamPath = path;
}

size_t KVStore::getMaxDatafileBytes() const {
    return maxDatafileBytes;
}

void KVStore::setMaxDatafileBytes(size_t maxBytes) {
    maxDatafileBytes = maxBytes;
}