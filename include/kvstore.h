#pragma once

#include <string>
#include <optional>
#include <filesystem>

struct Error {
    int val{0};
    std::string description;
};

struct Key {};
struct Value {};

struct Record {
    uint64_t timeStamp;
    uint32_t keySize;
    uint32_t valSize;
    Key key;
    Value val;
};

struct KVStoreHandle {
    int activeFileID{-1};
    std::string absDirPath;
    // hash table pointer.
};

class KVStore {

    public:
        KVStore();
        ~KVStore();

    private:

        /* Open a new or existing datastore with additional options */

        std::optional<KVStoreHandle> openStore(std::filesystem::path dirPath);

        /* 
            Open a new or existing Bitcask datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        std::optional<KVStoreHandle> openReadOnlyStore();

        void put(KVStoreHandle, Record rec);
};
