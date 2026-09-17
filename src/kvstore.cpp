#include "kvstore.h"

/* KVStore Methods */

std::filesystem::path KVStore::getDataDir() {
    return this->dataDir;
}

void KVStore::setDataDir(std::filesystem::path root) {
    this->dataDir = root.append("data/");
}

void KVStore::setActiveFilestream(std::ofstream stream) {
    this->activeFilestream = std::move(stream);
}

std::ofstream& KVStore::getActiveFilestream() {
    return this->activeFilestream;
}

KVStoreHandle KVStore::openStore(std::filesystem::path dirPath, StoreOptions sOptions) {

    KVStoreHandle stH; // Store handle.
    
    // if sOptions.syncOnPut ...
    // if sOptions.readWrite ... -> reading and writing permitted.


    // Setup variable, doesn't create dir.
    stH.setAbsDirPath();
    this->setDataDir(stH.getAbsDirPath());

    // Create dir if needed.
    if (!std::filesystem::exists(this->getDataDir()))
        std::filesystem::create_directories(this->getDataDir());

    // Get the active ID (highest) in dataDir.
    stH.setActiveFileID(this->getDataDir());

    // Turn active ID into string.
    std::stringstream ss;
    std::string strActiveFileID;
    ss << stH.getActiveFileID();
    ss >> strActiveFileID;

    // Filename as string.
    std::string datafileName = strActiveFileID + ".log";

    // Establish our KVStore stream.
    std::ofstream datafile (this->getDataDir() / datafileName);
    this->setActiveFilestream(std::move(datafile));
    datafile.close();

    return stH;
}

void KVStore::put(KVStoreHandle& stH, Record rec) {


    // data folder empty? or datafile dir empty? where is this stored. probably in KVStore.
    // if (!std::filesystem::exists(storeHandle.getAbsDirPath().append("/data/")))
    //     return;

    // lets assume our KVStoreHandle has been init properly with -1 by default.
    
    // Get the active datafile
    // if (stH.getActiveFileID() == -1ULL)
        // initialise a new file to write to.

        // need root dir.
        // int id = open();
        // storeHandle.setActiveFileID(2);
}