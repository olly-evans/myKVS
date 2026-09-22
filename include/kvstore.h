#pragma once

#include <string>
#include <fstream>

#include "kvstorehandle.h"

// constexpr uint8_t MAX_DATAFILE_BYTES = 4096;

// In other words, the first bit of a binary polynomial representation doesn’t carry any information about the polynomial when we agree on a fixed degree.

// For that reason, the first bit of a binary polynomial representation is always dropped when computing a CRC in software. So the bit size of the resulting binary representation is always n for a polynomial of degree n. Example:
class Record {
    private:
        uint32_t crc;
        uint32_t timeStamp;
        uint32_t keySize;
        uint32_t valSize;
        std::string key;
        std::string val;  
    public:
        Record() {
            // setCRC16();
        };

        void setTimestamp(uint32_t ts);
        uint32_t getTimestamp();

        void setKeySize(uint32_t ks);
        uint32_t getKeySize();

        void setValueSize(uint32_t vs);
        uint32_t getValueSize();

        void setKey(std::string k);
        std::string getKey();

        void setValue(std::string v);
        std::string getValue();
};

struct StoreFlags {
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

        KVStoreHandle openStore(std::filesystem::path relDataDir, StoreFlags stFlags);

        /* 
            Open a new or existing datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        // std::optional<KVStoreHandle> openReadOnlyStore();

        void put(const KVStoreHandle& h, const Record& rec);
};