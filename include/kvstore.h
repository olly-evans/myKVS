#pragma once

#include <unordered_map>
#include <mutex>
#include <shared_mutex>
#include <optional> 
#include <vector>

#include "kvstorehandle.h"

namespace fs = std::filesystem;

struct StoreFlags {
    bool readWrite = true; // Reading and writing permitted in directory.
    bool syncOnPut = false;

    size_t maxDatafileBytes = 65535;
    std::string datafileExtension;
};

class KVStore {

    public:
        // KVStore();
        // ~KVStore();

        /* Main API */

        KVStoreHandle open(fs::path relDataDir, StoreFlags stFlags); /* Open new or existing store in relDataDir. */
        void put(KVStoreHandle& stH, const std::string key, const std::string val);
        std::optional<std::string> get(const KVStoreHandle& stH, const std::string key);
        std::vector<std::string> listKeys(const KVStoreHandle& stH);
        void restore(KVStoreHandle& stH);


        /* File */

        [[nodiscard]] fs::path createDatafilePath(uint32_t fileID, std::string fileExtension) const;

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

        std::shared_mutex writeMutex;
        std::shared_mutex readMutex;
};