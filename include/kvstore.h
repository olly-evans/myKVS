#pragma once

#include <mutex>
#include <shared_mutex>
#include <optional> 
#include <vector>

#include "kvstorehandle.h"
#include "kverror.h"

namespace fs = std::filesystem;

struct StoreFlags {
    bool readWrite = true; // Reading and writing permitted in directory.
    bool syncOnPut = false;

    size_t maxDatafileBytes = 65535;
};

class KVStore {

    public:
        KVStore(const fs::path& dir);
        // ~KVStore();

        /* Main API */

        /* 
            Opens a new or existing store. relDataDir is a path relative to the project
            root (e.g. "data") — open() resolves it against the root and creates the
            directory if it doesn't already exist. 
        */

        [[nodiscard]] KVExpected open(const StoreFlags& flags); 

        /* Put a key/value pair to an existing data store. */
        [[nodiscard]] KVResult put(KVStoreHandle& stH, const std::string key, const std::string val);

        /* Retrieve data from an existing data store using a key */
        KVResult get(KVStoreHandle& stH, const std::string key);

        /* List all the keys in the keyDir */
        std::vector<std::string> listKeys(const KVStoreHandle& stH);

        /* Restore the keyDir from an existing store. Brute-force for now, no hint files. */
        void restore(KVStoreHandle& stH);
        
        /* Getters/Setters */

        [[nodiscard]] fs::path getRootDirPath() const;
        void updateRootDirPath();

        [[nodiscard]] fs::path getDataDir() const;        
        void setDataDir(fs::path dir);

        [[nodiscard]] std::ofstream& getActiveOutputStream();
        void setActiveOutputStream(std::ofstream stream);
        void updateActiveOutputStream(fs::path path);

        [[nodiscard]] std::ifstream& getActiveInputStream();
        void setActiveInputStream(std::ifstream& in);
        void updateActiveInputStream(fs::path path);

        [[nodiscard]] fs::path getActiveInputStreamPath() const;
        void setActiveInputStreamPath(fs::path path);

        [[nodiscard]] size_t getMaxDatafileBytes() const;
        void setMaxDatafileBytes(size_t maxBytes);

    private:
        std::shared_mutex rwMutex;

        fs::path absDirPath;
        fs::path dataDir;

        std::ofstream activeOutputStream;

        std::ifstream activeInputStream;
        fs::path activeInputStreamPath;

        size_t maxDatafileBytes;
};