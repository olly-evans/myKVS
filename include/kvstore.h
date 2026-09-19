#pragma once

#include <string>
#include <fstream>

#include "kvstorehandle.h"

// constexpr uint8_t MAX_DATAFILE_BYTES = 4096;

struct Record {
    uint32_t crc;
    uint32_t timeStamp;
    uint32_t keySize;
    uint32_t valSize;
    std::string key;
    std::string val;    
};

struct StoreOptions {
    bool readWrite = false; // Reading and writing permitted in directory.
    bool syncOnPut = false;
    std::string datafileExtension;
};

class KVStore {
    private:
        std::filesystem::path dataDir;
        std::ofstream activeFilestream;

    public:
        // KVStore();
        // ~KVStore();

        void setDataDir(std::filesystem::path dir, std::string dirName);
        std::filesystem::path getDataDir() const;

        void setActiveFilestream(std::ofstream stream);
        std::ofstream& getActiveFilestream();

        /* 
            Establish a new or existing datastore in CMAKE_SOURCE_DIR 
            Will eventually need to add additional options.           
        */

        KVStoreHandle openStore(std::filesystem::path relDataDir, StoreOptions sOptions);

        /* 
            Open a new or existing datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        // std::optional<KVStoreHandle> openReadOnlyStore();

        void put(const KVStoreHandle& h, const Record& rec);
};