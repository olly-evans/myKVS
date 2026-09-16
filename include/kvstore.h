#pragma once

#include <string>
#include <optional>
#include <filesystem>

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

// Seperate file.
class KVStoreHandle {
    private:
        uint64_t activeFileID;
        std::filesystem::path absDirPath;
        // hash table pointer.

    public:
        uint64_t getActiveFileID();
        void setActiveFileID(uint64_t newID);

        std::filesystem::path getAbsDirPath();
        void setAbsDirPath();
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

        std::optional<KVStoreHandle> openStore(std::filesystem::path dirPath);

        /* 
            Open a new or existing datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        std::optional<KVStoreHandle> openReadOnlyStore();

        void put(KVStoreHandle h, Record rec);
};