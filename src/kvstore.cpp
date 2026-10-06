
#include "detect_store.h"
#include "kvstore.h"

namespace fs = std::filesystem;

/* KVStore Methods */

KVStoreHandle KVStore::open(const fs::path pathDirRelativeToRoot, const StoreFlags stFlags) {

    // if stFlags.syncOnPut ... -> mutex in put function???
    // if stFlags.readWrite ... -> not fully fleshed out and tested
    // dataFileExt not changed right now. Just default to .data and this is a valid store file.

    KVStoreHandle stH;

    updateRootDirPath();
    setDataDir(getRootDirPath(), pathDirRelativeToRoot);    
    setMaxDatafileBytes(stFlags.maxDatafileBytes);
    stH.setDatafileExt(".data");
    stH.setReadWrite(stFlags.readWrite); 

    if (DetectStore::isStore(dataDir)) {

        stH.openActiveDatafile(dataDir, activeOutputStream);
        restore(stH);

        /* Set active datafile to be read-from by default. */
        updateActiveInputStream(stH.getActiveDatafilePath());

        std::cout << "Existing store opened successfully in:\n" << dataDir << std::endl;
        return stH;
    }

    /* Not an existing store. */
    fs::create_directories(dataDir);

    stH.openActiveDatafile(dataDir, activeOutputStream);
    /* Set active datafile to be read-from by default. */
    updateActiveInputStream(stH.getActiveDatafilePath()); 

    std::cout << "Store opened successfully in:\n" << dataDir << std::endl;
    return stH;
}

void KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    std::unique_lock<std::shared_mutex> lock(rwMutex);

    if (!fs::exists(dataDir)) {
        std::cout << "[WARNING] You must open a store before using put." << std::endl;
        return;
    }
    
    const fs::path df = stH.getActiveDatafilePath();

    if (!(fs::exists(df) && fs::is_regular_file(df))) {
        std::cout << "[WARNING] No active datafile path. Consider opening a store!" << std::endl;
        return;
    }

    uint32_t activePathID = stH.validDatafileToID(df);

    if (activePathID != stH.getActiveDatafileID()) {
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

    updateActiveOutputStream(stH.getActiveDatafilePath());
    
    rec.serialize(activeOutputStream);
    stH.updateKeyDir(stH.getActiveDatafilePath(), rec);
}

std::optional<std::string> KVStore::get(KVStoreHandle& stH, const std::string key) {

    std::shared_lock<std::shared_mutex> lock(rwMutex);

    auto it = stH.keyDir.find(key);
    if (it == stH.keyDir.end()) {
        return std::nullopt;
    }

    KeyDirEntry entry = it->second;

    fs::path readPath = stH.createDatafilePath(dataDir, entry.fileID, stH.getDatafileExt());
    if (!fs::exists(readPath)) 
        return std::nullopt;

    /* If input stream already reading from stH.getActiveDatafilePath(), no need to update the stream. */
    if (getActiveInputStreamPath().empty() || 
        !fs::equivalent(getActiveInputStreamPath(), readPath))
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
        return std::nullopt; // Mismatch between disk and expected crc.
    
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
            stH.updateKeyDir(df.path(), rec);
        }
    }
}

/* Getters and Setters */

fs::path KVStore::getRootDirPath() const {
    return absDirPath;
}

void KVStore::updateRootDirPath() {
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

fs::path KVStore::getActiveInputStreamPath() const {
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