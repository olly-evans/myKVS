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

        [[nodiscard]] KVStoreHandle open(fs::path relDataDir, StoreFlags stFlags); /* Open new or existing store in relDataDir. */
        void put(KVStoreHandle& stH, const std::string key, const std::string val);
        std::optional<std::string> get(const KVStoreHandle& stH, const std::string key);
        std::vector<std::string> listKeys(const KVStoreHandle& stH);
        void restore(KVStoreHandle& stH);

        /* Getters/Setters */

        [[nodiscard]] fs::path getAbsDirPath() const;
        void updateAbsDirPath();

        [[nodiscard]] fs::path getDataDir() const;        
        void setDataDir(fs::path dir, std::string dirName);

        [[nodiscard]] std::ofstream& getActiveOutputStream();
        void setActiveOutputStream(std::ofstream stream);
        void updateActiveOutputStream(fs::path path);

        [[nodiscard]] std::ifstream& getActiveInputStream();
        void setActiveInputStream(std::ifstream& in);
        void updateActiveInputStream(fs::path path);

        [[nodiscard]] fs::path getActiveInputStreamPath();
        void setActiveInputStreamPath(fs::path path);

        [[nodiscard]] size_t getMaxDatafileBytes() const;
        void setMaxDatafileBytes(size_t maxBytes);

    private:
        std::shared_mutex writeMutex;
        std::shared_mutex readMutex;

        fs::path absDirPath;
        fs::path dataDir;

        std::ofstream activeOutputStream;

        std::ifstream activeInputStream;
        fs::path activeInputStreamPath;

        size_t maxDatafileBytes;
};