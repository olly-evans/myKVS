#pragma once

#include <fstream>
#include <iostream>

#include "kvstorehandle.h"
#include "record.h"

constexpr uint16_t MAX_DATAFILE_BYTES = 65535;

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

        /* Main API */

        KVStoreHandle open(std::filesystem::path relDataDir, StoreFlags stFlags);
        void put(const KVStoreHandle& h, const std::string key, const std::string val);

        /* Getters/Setters */

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