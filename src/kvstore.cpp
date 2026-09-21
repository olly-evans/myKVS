#include "kvstore.h"

/* KVStore Methods */

std::filesystem::path KVStore::getDataDir() const {
    return this->dataDir;
}

void KVStore::setDataDir(std::filesystem::path root, std::string dirName) {
    this->dataDir = root.append(dirName);
}

void KVStore::setActiveFilestream(std::ofstream stream) {
    this->activeFilestream = std::move(stream);
}

std::ofstream& KVStore::getActiveFilestream() {
    return this->activeFilestream;
}

KVStoreHandle KVStore::openStore(const std::filesystem::path relDataDir, const StoreFlags sFlags) {

    
    KVStoreHandle stH; // Store handle.

    // store owns handle of course.
    
    // if sOptions.syncOnPut ... -> mutex in put function???
    // if sOptions.readWrite ... -> find way to allow read and write to dir.

    // Only one datastore process can write at once... dir must become a mutex to put().

    // Setup variable, doesn't create dir.
    stH.setAbsDirPath();
    this->setDataDir(stH.getAbsDirPath(), relDataDir);

    // Create dir if needed.
    if (!std::filesystem::exists(this->getDataDir()))
        std::filesystem::create_directories(this->getDataDir());

    stH.setDatafileExt(sFlags.datafileExtension);
    if (stH.getDatafileExt().empty())
        stH.setDatafileExt(".data");


    // Get the active ID (highest) in dataDir.
    stH.setActiveFileID(this->getDataDir());

    // Turn active ID into string.
    std::string strActiveFileID = std::to_string(stH.getActiveFileID());
    std::string datafileName = strActiveFileID + sFlags.datafileExtension;

    // Establish our KVStore stream.
    this->activeFilestream.open(
        this->getDataDir() / datafileName, 
        std::ios::binary | std::ios::app
    );
   
    return stH;
}

void KVStore::put(const KVStoreHandle& stH, const Record& rec) {

    // lock and mutex is bound to our KVStoreHandle. RAII

    // std::lock_guard<std::mutex> guard(mutex);

    // okay so Record is formatted beforehand when we take user input from terminal.

    // Do we have a data directory?
    // user must make one if not. return back to main loop perhaps.
    if (!std::filesystem::exists(this->getDataDir()))
        return;
    

    // ofstream and activefileid.
    
    // is file too big.
    
    // auto& out = this->getActiveFilestream();

    // // rec.crc = computeCRC32(rec.timeStamp, rec.keySize, rec.valSize, rec.key, rec.val);

    // out.write(reinterpret_cast<const char*>(&rec.crc), sizeof(rec.crc));
    // out.write(reinterpret_cast<const char*>(&rec.timeStamp), sizeof(rec.timeStamp));
    // out.write(reinterpret_cast<const char*>(&rec.keySize), sizeof(rec.keySize));
    // out.write(reinterpret_cast<const char*>(&rec.valSize), sizeof(rec.valSize));
    // out.write(reinterpret_cast<const char*>(&rec.key), rec.keySize);
    // out.write(reinterpret_cast<const char*>(&rec.val), rec.valSize);
    // out.close();

    // stH.setActiveFileID(this->dataDir);

    // append to hashtable.
}