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


        /* Getters/Setters */

        void setDataDir(fs::path dir, std::string dirName);
        [[nodiscard]] fs::path getDataDir() const;        

        void setActiveOutputStream(std::ofstream stream);
        [[nodiscard]] std::ofstream& getActiveOutputStream();

        [[nodiscard]] size_t getMaxDatafileBytes() const;
        void setMaxDatafileBytes(size_t maxBytes);

        [[nodiscard]] fs::path getAbsDirPath() const;
        void updateAbsDirPath();

    private:
        fs::path absDirPath;
        fs::path dataDir;

        size_t maxDatafileBytes;

        std::ofstream activeOutputStream;

        std::shared_mutex writeMutex;
        std::shared_mutex readMutex;
};