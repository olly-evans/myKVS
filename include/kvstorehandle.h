#pragma once

#include <filesystem>

// Seperate file.
class KVStoreHandle {
    private:
        uint32_t activeFileID;
        std::filesystem::path absDirPath;
        std::string datafileExtension;
        // hash table pointer.

    public:
        // KVStoreHandle(mutex.lock());
        // ~KVStoreHandle();

        uint32_t getActiveFileID();
        void setActiveFileID(std::filesystem::path dataDir);

        std::filesystem::path getAbsDirPath();
        void setAbsDirPath();

        std::string KVStoreHandle::getDatafileExt();
        void setDatafileExt(std::string fileExtension);

};