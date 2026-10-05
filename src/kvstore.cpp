
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

    if (DetectStore::isStore(dataDir)) {

        restore(stH);

        // FUNCTION
        stH.updateActiveDatafileID(dataDir);
        fs::path currentDatafilePath = stH.createDatafilePath(dataDir, 
                                                              stH.getActiveDatafileID(), 
                                                              stH.getDatafileExt()); 
        
        stH.setActiveDatafile(currentDatafilePath, activeOutputStream);
        
        /* Set to active datafile by default. */

        std::ifstream in(stH.getActiveDatafilePath(), std::ios::binary | std::ios::in);
        setActiveInputStream(in);
        setActiveInputStreamPath(stH.getActiveDatafilePath());

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

    std::ifstream in(stH.getActiveDatafilePath(), std::ios::binary | std::ios::in);
    setActiveInputStream(in);
    setActiveInputStreamPath(stH.getActiveDatafilePath());

    std::cout << "Store opened successfully in:\n" << dataDir << std::endl;

    return stH;
}

void KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    std::unique_lock<std::shared_mutex> lock(writeMutex);

    if (!fs::exists(dataDir)) {
        std::cout << "[WARNING] You must open a store before using put." << std::endl;
        return;
    }
    
    const fs::path df = stH.getActiveDatafilePath();

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

    /* OFC NOT WORKING IN TEST_GET, DOESNT USE OUR FILESTREAM. */
    // std::string valbuf = stH.readDiskValue(readPath, entry);

    // Record rec(key, valbuf); 
    // size_t crcDisk = stH.readDiskCRC(readPath, entry, rec);

    // if (rec.getCRC32() != crcDisk)
    //     return std::nullopt; /* Recommend deleting key {key} */
    
    return "asdf";
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

        // readRecord()

        updateActiveInputStream(df);
        // reading through whole file, so no seek required.

        while(activeInputStream.peek() != std::char_traits<char>::eof()) {
                        
            uint32_t crc = stH.readField<uint32_t>(activeInputStream);
            uint64_t timestamp = stH.readField<uint64_t>(activeInputStream);
            uint32_t keySize = stH.readField<uint32_t>(activeInputStream);
            uint32_t valSize = stH.readField<uint32_t>(activeInputStream);
            std::string key = stH.readString(activeInputStream, keySize);
            std::string val = stH.readString(activeInputStream, valSize);

            // Record rec(key, val);
            // // rec.setTimestamp();
            
            // // if (rec.byteSize() != bytesRead)
            // //     return;

            // stH.updateKeyDir(df.path(), rec);
            return;
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

void KVStore::setActiveInputStream(std::ifstream& in) {
    activeInputStream = std::move(in);
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