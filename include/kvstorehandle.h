#pragma once

#include <iostream>
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

        fs::path activeDatafilePath;
        std::string datafileExtension;
        uint32_t activeDatafileID;

        bool readWrite;

        std::unordered_map<std::string, KeyDirEntry> keyDir;

    public:
        // KVStoreHandle(mutex.lock());
        // ~KVStoreHandle();

        /* Datafile */
        [[nodiscard]] fs::path createDatafilePath(const fs::path& dataDir, uint32_t fID, std::string fExt) const;
        void setActiveDatafile(const fs::path& path, std::ofstream& out);
        void makeDatafileReadOnly(const fs::path& path, std::ofstream& out);
        void rollOverDatafile(const fs::path& dataDir, std::ofstream& out);

        template <typename T>
        T readField(std::istream& in);
        static std::string readString(std::istream& in, size_t n);

        void updateKeyDir(fs::path path, const Record rec);

        [[nodiscard]] fs::path getActiveDatafilePath() const;
        void setActiveDatafilePath(fs::path path);
        
        [[nodiscard]] uint32_t getActiveDatafileID() const;
        void updateActiveDatafileID(fs::path dataDir);

        [[nodiscard]] std::string getDatafileExt() const;
        void setDatafileExt(std::string fileExtension);

        [[nodiscard]] bool getReadWrite() const;
        void setReadWrite(bool readWrite);

    private:

        /* Testing */
        friend struct HandleTestAccess;
};

template <typename T>
T KVStoreHandle::readField(std::istream& in) {
    static_assert(std::is_trivially_copyable_v<T>);
    T v{};
    if (!in.read(reinterpret_cast<char*>(&v), sizeof(T)))
        throw std::runtime_error("Mismatching read sizes.");
    return v;
}