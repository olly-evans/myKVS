
#include "detect_store.h"
#include "kvstore.h"

/* KVStore Methods */

KVStore::KVStore(const fs::path& dir) : dataDir(dir) {};

KVExpected KVStore::open(const StoreFlags& flags) {

    /* DataFileExt not changed right now. Just default to .data and this is a valid store file. */
    /* Work is done beforehand in the main loop to deduce the root/relative path of dir. */

    KVStoreHandle stH;

    if (!dataDir.is_absolute()) {
        KVError err = {KVErrorCode::ProvidedPathNotAbsolute, 
                      "Path provided isn't absolute. Cannot open."}; 
        return std::unexpected(err);
    }

    setMaxDatafileBytes(flags.maxDatafileBytes);
    stH.setDatafileExt(".data");
    stH.setReadWrite(flags.readWrite); 

    if (fs::exists(dataDir) && DetectStore::isStore(dataDir) ) {

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

KVResult KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    std::unique_lock<std::shared_mutex> lock(rwMutex);

    if (!fs::exists(dataDir)) {
        KVError err = {KVErrorCode::StoreNotOpen, "No store open to write to."}; 
        return std::unexpected(err);
    }
    
    const fs::path df = stH.getActiveDatafilePath();

    if (!(fs::exists(df) && fs::is_regular_file(df))) {
        KVError err = {KVErrorCode::StoreNotOpen, "No active datafile path, consider opening a store."}; 
        return std::unexpected(err);
    }

    uint32_t activePathID = stH.validDatafilePathToID(df);
    if (activePathID != stH.getActiveDatafileID())
        throw std::runtime_error("Active datafile path and active ID mismatch - cannot proceed with write.");    

    if (!activeOutputStream.is_open() || activeOutputStream.bad())
        throw std::runtime_error("Output filestream is closed or in a bad state - cannot proceed with write.");
    
    Record rec(key, val);

    if (fs::file_size(df) + rec.byteSize() < getMaxDatafileBytes()) {
        // If program crash occurs between serialize and entry, flush() occurs and we can load from disk.
        stH.updateActiveDatafileID(dataDir);
        rec.serialize(activeOutputStream);
        stH.updateKeyDir(df, rec);

        return {};
    } 

    std::cout << "Datafile full, rolling-over..." << std::endl;

    stH.rollOverDatafile(dataDir, activeOutputStream);

    updateActiveOutputStream(stH.getActiveDatafilePath());
    
    rec.serialize(activeOutputStream);
    stH.updateKeyDir(stH.getActiveDatafilePath(), rec);

    return {};
}

KVResult KVStore::get(KVStoreHandle& stH, const std::string key) {

    std::shared_lock<std::shared_mutex> lock(rwMutex);

    auto it = stH.keyDir.find(key);
    if (it == stH.keyDir.end()) 
        return std::unexpected(KVError{KVErrorCode::KeyNotFound,
                                      "Key does not exist."});

    KeyDirEntry entry = it->second;

    // getReadPath().
    bool isReadPathActiveDatafile = entry.fileID == stH.getActiveDatafileID();
    fs::path oldDatafilePath = stH.createDatafilePath(dataDir, 
                                      entry.fileID, 
                                      DFStatus::ReadOnly, 
                                      stH.getDatafileExt());

    fs::path readPath = isReadPathActiveDatafile ? stH.getActiveDatafilePath() : oldDatafilePath; 

    if (!fs::exists(readPath)) 
        return std::unexpected(KVError{KVErrorCode::ReadPathDoesNotExist,
                                      "File containing key does not exist."});
    
    /* If input stream already reading from stH.getActiveDatafilePath(), no need to update the stream. */
    if (getActiveInputStreamPath().empty() || 
        !fs::equivalent(getActiveInputStreamPath(), readPath))
        updateActiveInputStream(readPath);

    // Don't read past eof.
    uintmax_t fileSize = fs::file_size(readPath);
    if ((entry.valFileOffset + entry.valSz) > fileSize) 
        return std::unexpected(KVError{KVErrorCode::ReadExceedsFileSize,
                                      "Value's offset plus its size is greater than the total file size."});

    activeInputStream.seekg(entry.valFileOffset, std::ios_base::beg);
    std::string val = stH.readString(activeInputStream, entry.valSz);

    Record rec(key, val);

    // Read CRC.
    std::streampos crcOff = activeInputStream.tellg() - static_cast<std::streampos>(rec.byteSize());
    activeInputStream.seekg(crcOff, std::ios_base::beg);
    uint32_t crc = stH.readField<uint32_t>(activeInputStream);

    if (rec.getExpectedCRC32(entry.tstamp) != crc)
        return std::unexpected(KVError{KVErrorCode::MismatchedCRC, 
                                      "CRC mismatch: data may be corrupt."});
    
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

void KVStore::setDataDir(fs::path dir) {
    dataDir = dir;
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