#pragma once

#include <string>
#include <optional>
#include <filesystem>

struct Error {
    uint8_t val{0};
    std::string description;
};

class Key {};
class Value {};

struct Record {    
    Key key;
    Value val;
    uint32_t keySize{-1};
    uint32_t valSize{-1};
    uint64_t timeStamp{-1};
};

class KVStoreHandle {
    private:
        uint64_t activeFileID{-1};
        std::filesystem::path absDirPath;
        // hash table pointer.

    public:
        uint64_t getActiveFileID();
        void setActiveFileID(uint64_t newID);

        std::filesystem::path getAbsDirPath();
        void setAbsDirPath();
};

class KVStore {

    public:
        KVStore();
        ~KVStore();

    private:

        /* Open a new or existing datastore with additional options */

        std::optional<KVStoreHandle> openStore(std::filesystem::path dirPath);

        /* 
            Open a new or existing datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        std::optional<KVStoreHandle> openReadOnlyStore();

        void put(KVStoreHandle h, Record rec);
};