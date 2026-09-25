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

    stH.setDatafileExt(stFlags.datafileExtension);
    if (stH.getDatafileExt().empty())
        stH.setDatafileExt(".data");

    // Get the active datafiles ID (highest filename) in dataDir.
    stH.updateActiveFileID(dataDir); 

    std::string strID = std::to_string(stH.getActiveFileID());
    fs::path appendOnlyDatafileExtension(strID + ".aol" + stH.getDatafileExt());
    activeDatafilePath = dataDir / appendOnlyDatafileExtension;

    stH.setReadWrite(stFlags.readWrite); 
    setActiveDatafile(activeDatafilePath, stFlags.readWrite);

    std::cout << "Store opened successfully in:\n" << dataDir << "\n";

    return stH;
}

void KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    // lock and mutex is bound to our KVStoreHandle. RAII

    if (!fs::exists(dataDir)) {
        std::cout << "[WARNING] You must open a store before using put." << "\n";
        return;
    }
    
    if (!(fs::exists(activeDatafilePath) && fs::is_regular_file(activeDatafilePath))) {
        std::cout << "[WARNING] No active datafile path. Consider opening a store!" << "\n";
        return;
    }

    std::string activePathID = activeDatafilePath.stem().stem();
    std::string activeHandleID = std::to_string(stH.getActiveFileID());

    if (activePathID != activeHandleID) {
        std::cout << "[ERROR] Active datafile doesn't match the active ID." << "\n";
        return;
    }

    if (!activeFilestream.is_open() || activeFilestream.bad())
        std::cout << "[ERROR] Filestream error!" << "\n";
    

    Record rec(key, val);

    if (fs::file_size(activeDatafilePath) + rec.byteSize() < MAX_DATAFILE_BYTES) {
        std::cout << "Active datafile found, serializing data." << "\n";
        rec.serialize(activeFilestream);
        return;
    } 

    // if its greater, make the new file and write. overlap is stupid and error prone.

    rollOverDatafile(stH);

    stH.updateActiveFileID(dataDir);

    rec.serialize(activeFilestream);
        
    // append to hashtable.
}

/* File */

fs::path KVStore::getNextDatafilePath(const KVStoreHandle& stH) {

    uint32_t newID = stH.getActiveFileID() + 1;
    std::string newDatafileID = std::to_string(newID);
    std::string newDatafileName = newDatafileID + ".aol" + stH.getDatafileExt();

    fs::path nextDatafilePath = dataDir / newDatafileName;
    return nextDatafilePath;
}

void KVStore::setActiveDatafile(fs::path path, bool readWrite) {

 
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

    fs::path readOnlyExtension(".rol" + stH.getDatafileExt());
    activeDatafilePath.replace_extension(readOnlyExtension);

    fs::permissions(activeDatafilePath,
                    fs::perms::owner_read | fs::perms::group_read, 
                    fs::perm_options::replace);


    activeFilestream.close(); // Close old file.

    fs::path newDatafilePath = getNextDatafilePath(stH);
    setActiveDatafile(newDatafilePath, stH.getReadWrite());

    return;
}

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