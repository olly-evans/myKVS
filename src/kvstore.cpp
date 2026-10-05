
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

        // stH.updateActiveDatafile(dataDir, activeOutputStream);
        stH.updateActiveDatafileID(dataDir);
        fs::path currentDatafilePath = stH.createDatafilePath(dataDir, 
                                                              stH.getActiveDatafileID(), 
                                                              stH.getDatafileExt()); 
        
        stH.setActiveDatafile(currentDatafilePath, activeOutputStream);

        restore(stH);

        /* Set to active datafile by default. */

        updateActiveInputStream(stH.getActiveDatafilePath());

        std::cout << "Existing store opened successfully in:\n << dataDir" << std::endl;
        return stH;
    }

    fs::create_directories(dataDir);

    // stH.updateActiveDatafile(dataDir, activeOutputStream);
    stH.updateActiveDatafileID(dataDir);
    fs::path currentDatafilePath = stH.createDatafilePath(dataDir, 
                                                          stH.getActiveDatafileID(), 
                                                          stH.getDatafileExt()); 
    
    stH.setActiveDatafile(currentDatafilePath, activeOutputStream);

    /* Set to active datafile to read by default at beginning. */
    updateActiveInputStream(stH.getActiveDatafilePath());

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
        stH.updateActiveDatafileID(dataDir);
        rec.serialize(activeOutputStream);
        stH.updateKeyDir(df, rec);
        return;
    } 

    std::cout << "Datafile full, rolling-over..." << std::endl;

    stH.rollOverDatafile(dataDir, activeOutputStream);
    stH.updateActiveDatafileID(dataDir);

    updateActiveOutputStream(stH.getActiveDatafilePath());
    
    rec.serialize(activeOutputStream);
    stH.updateKeyDir(stH.getActiveDatafilePath(), rec);
}

std::optional<std::string> KVStore::get(KVStoreHandle& stH, const std::string key) {

    std::shared_lock<std::shared_mutex> lock(readMutex);

    auto it = stH.keyDir.find(key);
    if (it == stH.keyDir.end()) {
        return std::nullopt;
    }

    KeyDirEntry entry = it->second;

    std::cout << entry.fileID << "\n"; // ZEROO?????

    fs::path readPath = stH.createDatafilePath(dataDir, entry.fileID, stH.getDatafileExt());
    if (!fs::exists(readPath)) 
        return std::nullopt;

    /* if input stream already reading from stH.getActiveDatafilePath(), no set */
    if (getActiveInputStreamPath().empty() || (getActiveInputStreamPath() != readPath))
        updateActiveInputStream(readPath);

    uintmax_t fileSize = fs::file_size(readPath);
    if ((entry.valFileOffset + entry.valSz) > fileSize)
        return std::nullopt; // Woud read past the file size.

    activeInputStream.seekg(entry.valFileOffset, std::ios_base::beg);
    std::string val = stH.readString(activeInputStream, entry.valSz);

    Record rec(key, val);

    // Read CRC.
    std::streampos crcOff = activeInputStream.tellg() - static_cast<std::streampos>(rec.byteSize());
    activeInputStream.seekg(crcOff, std::ios_base::beg);
    uint32_t crc = stH.readField<uint32_t>(activeInputStream);
    
    // Calculate expected CRC. rec.getExpectedCRC();
    rec.setTimestamp(entry.tstamp);
    rec.setCRC32();

    if (rec.getCRC32() != crc)
        return std::nullopt; // CRC mismatch from disk and expected.
    
    return val;
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

        updateActiveInputStream(df);

        while(activeInputStream.peek() != std::char_traits<char>::eof()) {
            
            /* Zero offset as we're reading through whole file here sequentially. */
            Record rec = stH.readRecord(activeInputStream, 0);
            stH.updateKeyDir(df.path(), rec); // needs to take id
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

void KVStore::updateActiveOutputStream(fs::path path) {

    if (activeOutputStream.is_open())
        activeOutputStream.close();
    
    activeOutputStream.clear();

    activeOutputStream.open(path, std::ios::binary | std::ios::app);
}

std::ifstream& KVStore::getActiveInputStream() {
    return activeInputStream;
}

void KVStore::setActiveInputStream(std::ifstream& in) {
    activeInputStream = std::move(in);
}

void KVStore::updateActiveInputStream(fs::path path) {
    
    if (activeInputStream.is_open()) 
        activeInputStream.close();

    activeInputStream.clear();

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