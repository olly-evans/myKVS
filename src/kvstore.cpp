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

    setActiveDatafile(activeDatafilePath, stFlags.readWrite);


    return stH;
}

void KVStore::put(KVStoreHandle& stH, const std::string key, const std::string val) {

    // lock and mutex is bound to our KVStoreHandle. RAII

    if (!fs::exists(dataDir)) {
        std::cout << "[WARNING] You must open a store before using put." << "\n";
        return;
    }
    
    Record rec(key, val);

    // if file is not full and the active id matches filename.

    // are we writing to the correct file.

    if (activeDatafilePath.filename().string() == std::to_string(stH.getActiveFileID()))
        // really bad.

    if (fs::file_size(activeDatafilePath) + rec.byteSize() < MAX_DATAFILE_BYTES) {
        rec.serialize(activeFilestream);
    } 

    // if its greater, make the new file and write. overlap is stupid and error prone.

    fs::path newDatafilePath = getNextDatafilePath(stH);
    rollOverDatafile();

    stH.updateActiveFileID(dataDir);
    activeDatafilePath = newDatafilePath;
    rec.serialize(activeFilestream);
    
    // don't write.

    // create new file


    // setActiveFileID
    

    // calculate the crc first so we can check how many total bytes to append
    // total bytes.
    // and if we need a new datafile.

    // in put(), don't need to roll over in open().
    // rollOverDatafile(); // calls createDatafile() within.

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

    return;
}

void KVStore::rollOverDatafile() {

    // perhaps tweak old file permissions/extension before we close filestream.


    activeFilestream.close(); // Close old file.

    // fs::path readOnlyExtension(".rol" + stH.getDatafileExt());
    // currentDatafilePath.replace_extension(readOnlyExtension);

    // Replace old datafile extension with .
    // Make old file read-only.
    // fs::permissions(currentDatafilePath,
    //                 fs::perms::owner_read | fs::perms::group_read | 
    //                 fs::perms::others_read, fs::perm_options::replace);

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

fs::path KVStore::getActiveDatafilePath() {
    return activeDatafilePath;
}