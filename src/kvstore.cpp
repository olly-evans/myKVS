#include "kvstore.h"

using namespace std;

/* KVStore Methods */


filesystem::path KVStore::getDataDir() const {
    return dataDir;
}

void KVStore::setDataDir(filesystem::path root, string dirName) {
    dataDir = root.append(dirName);
}

void KVStore::setActiveFilestream(ofstream stream) {
    activeFilestream = move(stream);
}

std::ofstream& KVStore::getActiveFilestream() {
    return activeFilestream;
}

KVStoreHandle KVStore::openStore(const filesystem::path relDataDir, const StoreFlags stFlags) {


    KVStoreHandle stH;
    
    // if stFlags.syncOnPut ... -> mutex in put function???
    // if stFlags.readWrite ... -> find way to allow read and write to dir.

    stH.setAbsDirPath();
    setDataDir(stH.getAbsDirPath(), relDataDir);

    // Create dir if needed.
    if (!filesystem::exists(getDataDir()))
        filesystem::create_directories(getDataDir());

    stH.setDatafileExt(stFlags.datafileExtension);
    if (stH.getDatafileExt().empty())
        stH.setDatafileExt(".data");

    // Get the active datafiles ID (highest filename) in dataDir.
    stH.setActiveFileID(getDataDir());

    // createNewDatafile()
    string strActiveFileID = to_string(stH.getActiveFileID());
    string datafileName = strActiveFileID + stFlags.datafileExtension;

    activeFilestream.open(
        getDataDir() / datafileName, 
        ios::binary | ios::app
    );
   
    return stH;
}

void KVStore::put(const KVStoreHandle& stH, const string key, const string val) {

    // lock and mutex is bound to our KVStoreHandle. RAII

    // Do we have a data directory?
    // user must make one if not. return back to main loop perhaps.
    if (!filesystem::exists(getDataDir())) {
        cout << "[WARNING] Cannot use put as there is no open store." << "\n";
        return;
    }
    
    Record rec(key, val);

    // if file is not full and the active id matches filename.
    rec.serialize(getActiveFilestream());
    // ofstream and activefileid.

    // else
    // create new file
    // setActiveFileID
 

    // calculate the crc first so we can check how many total bytes to append
    // total bytes.
    // and if we need a new datafile.

    // append to hashtable.
}