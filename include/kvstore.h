#pragma once

#include <fstream>
#include <iostream>
#include <unordered_map>

#include "kvstorehandle.h"
#include "record.h"

namespace fs = std::filesystem;

constexpr uint16_t MAX_DATAFILE_BYTES = 65535;

struct StoreFlags {
    bool readWrite = true; // Reading and writing permitted in directory.
    bool syncOnPut = false;

    std::string datafileExtension;
};

struct KeyDirEntry {
    uint32_t fileID;
    uint32_t valSz;
    uint64_t offset;
    uint64_t tstamp;
};

class KVStore {

    public:
        // KVStore();
        // ~KVStore();

        

        /* Main API */

        KVStoreHandle open(fs::path relDataDir, StoreFlags stFlags); /* Open new or existing store in relDataDir. */
        void put(KVStoreHandle& h, const std::string key, const std::string val);
        std::string KVStore::get(const KVStoreHandle& stH, std::string key);

        /* File */

        [[nodiscard]] fs::path getNextDatafilePath(const KVStoreHandle& stH);

        void setActiveDatafile(fs::path path, bool readWrite);
        void rollOverDatafile(const KVStoreHandle& stH);

        /* Getters/Setters */

        void setDataDir(fs::path dir, std::string dirName);
        [[nodiscard]] fs::path getDataDir() const;

        void setActiveFilestream(std::ofstream stream);
        [[nodiscard]] std::ofstream& getActiveFilestream();

        void setActiveDatafilePath(fs::path path);
        [[nodiscard]] fs::path getActiveDatafilePath() const;

    private:
        fs::path dataDir;
        fs::path activeDatafilePath;
        std::ofstream activeFilestream;

        std::unordered_map<std::string, KeyDirEntry> keyDir;
};