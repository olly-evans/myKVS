#pragma once

#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <fstream>

#include "record.h"

namespace fs = std::filesystem;

enum class DFStatus {Active, ReadOnly};

struct KeyDirEntry {
    uint32_t fileID;
    uint32_t valSz;
    uint64_t valFileOffset;
    uint64_t tstamp;
};

class KVStoreHandle {

    public:
        // KVStoreHandle(mutex.lock());
        // ~KVStoreHandle();

        /* Datafile */

        void openActiveDatafile(fs::path dataDir, std::ofstream& out);

        [[nodiscard]] fs::path createDatafilePath(const fs::path& dataDir, 
                                                  uint32_t fID,  
                                                  DFStatus stat, 
                                                  std::string fExt) const;

        void setActiveDatafile(const fs::path& path, std::ofstream& out);
        void makeDatafileReadOnly(const fs::path& path, std::ofstream& out);
        void rollOverDatafile(const fs::path& dataDir, std::ofstream& out);

        [[nodiscard]] uint32_t validDatafilePathToID(fs::path path);

        template <typename T>
        [[nodiscard]] T readField(std::ifstream& in);
        [[nodiscard]] static std::string readString(std::ifstream& in, size_t n);
        [[nodiscard]] Record readRecord(std::ifstream& in, const std::streamoff recFileOffset);

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
        friend class KVStore;


        fs::path activeDatafilePath;
        std::string datafileExtension;
        uint32_t activeDatafileID;

        bool readWrite;

        std::unordered_map<std::string, KeyDirEntry> keyDir;

    private:

        /* Testing */
        friend struct HandleTestAccess;


};

template <typename T>
T KVStoreHandle::readField(std::ifstream& in) {
    static_assert(std::is_trivially_copyable_v<T>);
    T v{};
    if (!in.read(reinterpret_cast<char*>(&v), sizeof(T)))
        throw std::runtime_error("Mismatching read sizes.");
    return v;
}