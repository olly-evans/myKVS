#pragma once

#include <string>
#include <fstream>
#include <mutex>

#include "kvstorehandle.h"

// constexpr uint8_t MAX_DATAFILE_BYTES = 4096;

class Key {};
class Value {};

struct Record {    
    Key key;
    Value val;

    uint32_t keySize;
    uint32_t valSize;
    uint64_t timeStamp;
};

struct StoreOptions {
    bool readWrite = false; // Reading and writing permitted in directory.
    bool syncOnPut = false;
};

class KVStore {
    private:
        std::filesystem::path dataDir;
        std::ofstream activeFilestream;
        std::mutex putLock;

    public:
        // KVStore();
        // ~KVStore();

        void setDataDir(std::filesystem::path dir);
        std::filesystem::path getDataDir();

        void setActiveFilestream(std::ofstream stream);
        std::ofstream& getActiveFilestream();

        /* 
            Establish a new or existing datastore in CMAKE_SOURCE_DIR 
            Will eventually need to add additional options.           
        */

        KVStoreHandle openStore(std::filesystem::path dirPath, StoreOptions sOptions);

        /* 
            Open a new or existing datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        // std::optional<KVStoreHandle> openReadOnlyStore();

        void put(KVStoreHandle& h, Record rec);
};