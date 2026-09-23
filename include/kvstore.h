#pragma once

#include <fstream>
#include <iostream>

#include "kvstorehandle.h"
#include "record.h"

// constexpr uint8_t MAX_DATAFILE_BYTES = 4096;

// In other words, the first bit of a binary polynomial representation doesn’t carry any information about the polynomial when we agree on a fixed degree.

// For that reason, the first bit of a binary polynomial representation is always dropped when computing a CRC in software. So the bit size of the resulting binary representation is always n for a polynomial of degree n. Example:


struct StoreFlags {
    bool readWrite = false; // Reading and writing permitted in directory.
    bool syncOnPut = false;
    std::string datafileExtension;
};

class KVStore {

    public:
        // KVStore();
        // ~KVStore();

        /* 
            Establish a new or existing datastore in CMAKE_SOURCE_DIR 
            Will eventually need to add additional options.           
        */

        KVStoreHandle open(std::filesystem::path relDataDir, StoreFlags stFlags);

        /* 
            Open a new or existing datastore for read-only access.
            The directory and all files in it must be readable by this process.
        */

        // std::optional<KVStoreHandle> openReadOnlyStore();

        void put(const KVStoreHandle& h, const std::string key, const std::string val);

        void setDataDir(std::filesystem::path dir, std::string dirName);
        [[nodiscard]] std::filesystem::path getDataDir() const;

        void setActiveFilestream(std::ofstream stream);
        [[nodiscard]] std::ofstream& getActiveFilestream();

        void setActiveDatafilePath(std::filesystem::path path);
        [[nodiscard]] std::filesystem::path getActiveDatafilePath();

    private:
        std::filesystem::path dataDir;
        std::filesystem::path activeDatafilePath;

        std::ofstream activeFilestream;
};