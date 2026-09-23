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
    // okay so Record is formatted beforehand when we take user input from terminal.

    // Do we have a data directory?
    // user must make one if not. return back to main loop perhaps.
    if (!filesystem::exists(getDataDir()))
        return;
    
    Record rec(key, val);
    // ofstream and activefileid.
    
    // is file too big.
    
    ofstream& out = getActiveFilestream();

    // while (activefilelen != eof) {
    //     out.write()
    // }

    // calculate the crc first so we can check how many total bytes to append
    // total bytes.
    // and if we need a new datafile.

    // if fileid = activestream.something_file_related()

    // out.write(reinterpret_cast<const char*>(rec.crc), sizeof(rec.crc));
    out.write(reinterpret_cast<const char*>(rec.getTimestamp()), sizeof(rec.getTimestamp()));
    out.write(reinterpret_cast<const char*>(rec.getKeySize()), sizeof(rec.getKeySize()));
    out.write(reinterpret_cast<const char*>(rec.getValueSize()), sizeof(rec.getValueSize()));
    out.write((rec.getKey().data()), rec.getKeySize());
    out.write((rec.getValue().data()), rec.getValueSize());


    // append to hashtable.
}