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

KVStoreHandle KVStore::openStore(std::filesystem::path relDataDir, StoreOptions sOptions) {

    KVStoreHandle stH; // Store handle.
    
    // if sOptions.syncOnPut ... -> mutex in put function???
    // if sOptions.readWrite ... -> find way to allow read and write to dir.

    // Only one datastore process can write at once... dir must become a mutex to put().

    // Setup variable, doesn't create dir.
    stH.setAbsDirPath();
    this->setDataDir(stH.getAbsDirPath(), relDataDir);

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
    std::ofstream datafile (this->getDataDir() / datafileName, std::ofstream::binary);
    this->setActiveFilestream(std::move(datafile));
    datafile.close();

    return stH;
}

void KVStore::put(const KVStoreHandle& stH, const Record& rec) {

    // lock and mutex is bound to our KVStoreHandle. RAII

    // std::lock_guard<std::mutex> guard(mutex);

    // okay so Record is formatted beforehand when we take user input from terminal.

    // Do we have a data directory?
    if (!std::filesystem::exists(this->getDataDir()))
        return;
    
    // we do have a data directory.

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