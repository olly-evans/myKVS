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
    stH.updateActiveFileID(dataDir); 

    fs::path currentDatafilePath = createDatafilePath(stH.getActiveFileID(), stH.getDatafileExt()); 
    
    stH.setReadWrite(stFlags.readWrite); 
    setActiveDatafile(currentDatafilePath, stFlags.readWrite);

    std::cout << "Store opened successfully in:\n" << dataDir << std::endl;

    return stH;
}

void KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    // lock and mutex is bound to our KVStoreHandle. RAII

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
        std::cout << "[ERROR] Active datafile doesn't match the active ID." << std::endl;
        return;
    }

    if (!activeFilestream.is_open() || activeFilestream.bad())
        std::cout << "[ERROR] Filestream error!" << std::endl;
    

    Record rec(key, val);

    if (fs::file_size(activeDatafilePath) + rec.byteSize() < stH.getMaxDatafileBytes()) {
        // If program crash occurs between serialize and entry, flush() occurs and we can load from disk.
        putRecord(rec, stH.getActiveFileID());
        return;
    } 

    // if its greater, make the new file and write. overlap is stupid and error prone.

    std::cout << "Datafile full, rolling-over..." << std::endl;

    rollOverDatafile(stH);
    stH.updateActiveFileID(dataDir);
        
    putRecord(rec, stH.getActiveFileID());    
}

std::string KVStore::get(const KVStoreHandle& stH, std::string key) {
    // KeyDirEntry entry = keyDir.at(key);
    
    // file could be aol or rol.
    // entry.fileID
    return "foo";
}

void KVStore::putRecord(const Record rec, const uint32_t datafileID) {
        
    rec.serialize(activeFilestream);

    size_t fileBytes = fs::file_size(activeDatafilePath);
    uint64_t valueByteOffset = fileBytes - rec.getValueSize();

    // This operator behaves funnily but forgot how.
    keyDir[rec.getKey()] = KeyDirEntry{datafileID,
                                       rec.getValueSize(),
                                       valueByteOffset,
                                       rec.getTimestamp()};
    
    std::cout << "Successfully serialized data to " << 
                    activeDatafilePath.filename()     <<
                    "\nKeyDir entries is: "           << 
                    keyDir.size()                     <<
                    std::endl;
}

/* File */

fs::path KVStore::createDatafilePath(uint32_t fileID, std::string fileExtension) {
    
    std::string strID = std::to_string(fileID);
    fs::path appendOnlyDatafileExtension(strID + ".aol" + fileExtension);
    activeDatafilePath = dataDir / appendOnlyDatafileExtension;
    return activeDatafilePath;

}

void KVStore::setActiveDatafile(const fs::path path, const bool readWrite) {

 
    fs::perms readWritePerms = fs::perms::owner_write | fs::perms::group_write | 
                               fs::perms::owner_read  |  fs::perms::group_read;

    fs::perms readOnlyPerms = fs::perms::owner_read  |  
                              fs::perms::group_read;

    activeFilestream.open(path, std::ios::binary | std::ios::app);

    fs::perms fPermissions = readWrite ? readWritePerms : readOnlyPerms;
    fs::permissions(path, fPermissions, fs::perm_options::replace);

    setActiveDatafilePath(path);

    return;
}

void KVStore::rollOverDatafile(const KVStoreHandle& stH) {

    fs::path newPath = activeDatafilePath;
    std::string filename = newPath.filename().string();
    filename.replace(filename.find("aol"), 3, "rol"); 
    newPath.replace_filename(filename);

    fs::rename(activeDatafilePath, newPath);

    fs::permissions(newPath,
                    fs::perms::owner_read | fs::perms::group_read, 
                    fs::perm_options::replace);


    activeFilestream.close(); // Close old file.

    fs::path nextDatafilePath = createDatafilePath(stH.getActiveFileID() + 1, stH.getDatafileExt());
    setActiveDatafile(nextDatafilePath, stH.getReadWrite());

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