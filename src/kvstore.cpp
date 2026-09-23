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

    // createNewDatafile()
    std::string strActiveFileID = std::to_string(stH.getActiveFileID());
    std::string datafileName = strActiveFileID + stFlags.datafileExtension;

    // setActiveDatafilePath(getDataDir() / datafileName);

    activeFilestream.open(
        getDataDir() / datafileName, 
        std::ios::binary | std::ios::app
    );
   
    return stH;
}

void KVStore::put(const KVStoreHandle& stH, const std::string key, const std::string val) {

    // lock and mutex is bound to our KVStoreHandle. RAII

    // Do we have a data directory?
    // user must make one if not. return back to main loop perhaps.
    if (!fs::exists(getDataDir())) {
        std::cout << "[WARNING] You must create an open store before using put." << "\n";
        return;
    }
    
    Record rec(key, val);

    // if file is not full and the active id matches filename.
    
    if (fs::file_size(getActiveDatafilePath()) < MAX_DATAFILE_BYTES)
        rec.serialize(activeFilestream);
    
    // ofstream and activefileid.

    // else
    // create new file
    // setActiveFileID
 

    // calculate the crc first so we can check how many total bytes to append
    // total bytes.
    // and if we need a new datafile.

    // append to hashtable.
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

void KVStore::setActiveDatafilePath(std::filesystem::path path) {
    activeDatafilePath = path;
}

std::filesystem::path KVStore::getActiveDatafilePath() {
    return activeDatafilePath;
}