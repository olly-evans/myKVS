#pragma once

#include <string>

#include "kvstorehandle.h"

struct Error {
    uint8_t val;
    std::string description;
};

class Key {};
class Value {};

struct Record {    
    Key key;
    Value val;
    uint32_t keySize;
    uint32_t valSize;
    uint64_t timeStamp;
};



class KVStore {
    private:
        std::filesystem::path dataDir;

    public:
        // KVStore();
        // ~KVStore();

        void setDataDir(std::filesystem::path dir);
        std::filesystem::path getDataDir();

        /* Open a new or existing datastore with additional options */

        KVStoreHandle openStore(std::filesystem::path dirPath);

        /* 
            Open a new or existing datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        // std::optional<KVStoreHandle> openReadOnlyStore();

        void put(KVStoreHandle h, Record rec);
};