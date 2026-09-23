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
    if (!fs::exists(getDataDir()))
        fs::create_directories(getDataDir());

    stH.setDatafileExt(stFlags.datafileExtension);
    if (stH.getDatafileExt().empty())
        stH.setDatafileExt(".data");

    // Get the active datafiles ID (highest filename) in dataDir.
    stH.setActiveFileID(getDataDir());

    createSetNewDatafile(stH);
   
    return stH;
}

void KVStore::put(const KVStoreHandle& stH, const std::string key, const std::string val) {

    // lock and mutex is bound to our KVStoreHandle. RAII

    // Do we have a data directory?
    if (!fs::exists(getDataDir())) {
        std::cout << "[WARNING] You must create an open store before using put." << "\n";
        return;
    }
    
    Record rec(key, val);

    // if file is not full and the active id matches filename.

    // size of total write.
    // are we writing to the correct file.

    // bool matchingFileID = stH.getActiveFileID() == std::to_integer(getActiveDatafilePath().filename());

    if (fs::file_size(getActiveDatafilePath()) < MAX_DATAFILE_BYTES) {
        rec.serialize(activeFilestream);
    } 

    // file is too big.

    // don't write.

    // create new file
    // setActiveFileID
    

    // calculate the crc first so we can check how many total bytes to append
    // total bytes.
    // and if we need a new datafile.

    // append to hashtable.
}

void KVStore::createSetNewDatafile(KVStoreHandle& stH) {

    // Both as we can be writing to a non-full 0 ID file.
    bool noDatafiles = stH.getActiveFileID() == 0 && fs::is_empty(dataDir);
    
    uint32_t newID = noDatafiles ? stH.getActiveFileID() : stH.getActiveFileID() + 1;

    std::string newDatafileID = std::to_string(newID);
    std::string newDatafileName = newDatafileID + ".aol" + stH.getDatafileExt();

    fs::path newDatafilePath = dataDir / newDatafileName;

    // noDatafiles should mean no activeFilestream file.
    if (noDatafiles && !activeFilestream.is_open()) {

        setActiveDatafilePath(newDatafilePath);
        activeFilestream.open(newDatafilePath, 
                              std::ios::binary | std::ios::app);
        
        fs::permissions(newDatafilePath,
                        fs::perms::owner_write | fs::perms::group_write | 
                        fs::perms::others_write, fs::perm_options::replace);

        // Don't need to setActiveFileID here, remains zero.
        return;
    }

    // We have an active datafile.
    
    activeFilestream.close(); // Close old file.

    fs::path currentDatafilePath = getActiveDatafilePath();

    // Replace old datafile extension with .rol.
    fs::path readOnlyExtension(".rol" + stH.getDatafileExt());
    currentDatafilePath.replace_extension(readOnlyExtension);

    // Make old file read-only.
    fs::permissions(currentDatafilePath,
                    fs::perms::owner_read | fs::perms::group_read | 
                    fs::perms::others_read, fs::perm_options::replace);

    // Open stream with new file.
    activeFilestream.open(newDatafilePath, 
                            std::ios::binary | std::ios::app);

    // Set new datafile path and scan for the new highest ID in the dir.
    // We scan the dir incase we reload executable and need to rebuild.
    setActiveDatafilePath(newDatafilePath);
    stH.setActiveFileID(dataDir);

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