#pragma once

#include <fstream>
#include <iostream>
#include <unordered_map>
#include <mutex>
#include <optional> 

#include "kvstorehandle.h"
#include "record.h"

namespace fs = std::filesystem;

struct StoreFlags {
    bool readWrite = true; // Reading and writing permitted in directory.
    bool syncOnPut = false;

    size_t maxDatafileBytes = 65535;
    std::string datafileExtension;
};

struct KeyDirEntry {
    uint32_t fileID;
    uint32_t valSz;
    uint64_t valFileOffset;
    uint64_t tstamp;
};

class KVStore {

    public:
        // KVStore();
        // ~KVStore();

        /* Main API */

        KVStoreHandle open(fs::path relDataDir, StoreFlags stFlags); /* Open new or existing store in relDataDir. */
        void put(KVStoreHandle& stH, const std::string key, const std::string val);
        std::optional<std::string> get(const KVStoreHandle& stH, const std::string key);

        void putRecord(const Record rec, const uint32_t datafileID);

        /* File */

        [[nodiscard]] fs::path createDatafilePath(uint32_t fileID, std::string fileExtension);

        void setActiveDatafile(const fs::path path, const bool readWrite);
        void rollOverDatafile(const KVStoreHandle& stH);
        void makeDatafileReadOnly(fs::path path);

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

        std::mutex putMut;

    public:

        /* Testing */
        
        friend void test_put(fs::path dir);
        friend void test_put_threads(fs::path dir);
        friend void test_get_corrupt_data(fs::path dir);


};