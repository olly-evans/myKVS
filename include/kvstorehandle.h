#pragma once

#include <filesystem>

// Seperate file.
class KVStoreHandle {
    private:
        uint32_t activeFileID;
        std::filesystem::path absDirPath;
        // hash table pointer.

    public:
        uint32_t getActiveFileID();
        void setActiveFileID(std::filesystem::path dataDir);

        std::filesystem::path getAbsDirPath();
        void setAbsDirPath();
};