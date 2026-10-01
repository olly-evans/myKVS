#pragma once

#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "record.h"

namespace fs = std::filesystem;

struct KeyDirEntry {
    uint32_t fileID;
    uint32_t valSz;
    uint64_t valFileOffset;
    uint64_t tstamp;
};

class KVStoreHandle {

    friend class KVStore;

    private:
        uint32_t activeFileID;
        size_t maxDatafileBytes;

        fs::path absDirPath;
        std::string datafileExtension;

        bool readWrite;

        std::unordered_map<std::string, KeyDirEntry> keyDir;

    public:
        // KVStoreHandle(mutex.lock());
        // ~KVStoreHandle();

        [[nodiscard]] uint32_t readCRC(fs::path path, KeyDirEntry entry, Record rec) const;
        [[nodiscard]] std::string readValue(fs::path path, KeyDirEntry entry) const;

        [[nodiscard]] uint32_t getActiveFileID() const;
        void updateActiveDatafileID(fs::path dataDir);

        [[nodiscard]] size_t getMaxDatafileBytes() const;
        void setMaxDatafileBytes(size_t maxBytes);

        [[nodiscard]] fs::path getAbsDirPath() const;
        void setAbsDirPath();

        [[nodiscard]] std::string getDatafileExt() const;
        void setDatafileExt(std::string fileExtension);

        [[nodiscard]] bool getReadWrite() const;
        void setReadWrite(bool readWrite);

    private:

        /* Testing */
        friend struct HandleTestAccess;
};