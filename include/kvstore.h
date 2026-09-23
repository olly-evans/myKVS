#pragma once

#include <fstream>
#include <iostream>

#include "kvstorehandle.h"
#include "record.h"
namespace fs = std::filesystem;

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

        KVStoreHandle open(fs::path relDataDir, StoreFlags stFlags);
        void put(const KVStoreHandle& h, const std::string key, const std::string val);

        /* File */

        [[nodiscard]] fs::path getNextDatafilePath(const KVStoreHandle& stH);

        void setActiveDatafile(fs::path path);
        void restoreActiveDatafile(fs::path path);

        void createSetNewDatafile(KVStoreHandle& stH);

        /* Getters/Setters */

        void setDataDir(fs::path dir, std::string dirName);
        [[nodiscard]] fs::path getDataDir() const;

        void setActiveFilestream(std::ofstream stream);
        [[nodiscard]] std::ofstream& getActiveFilestream();

        void setActiveDatafilePath(fs::path path);
        [[nodiscard]] fs::path getActiveDatafilePath();

    private:
        fs::path dataDir;
        fs::path activeDatafilePath;

        std::ofstream activeFilestream;
};