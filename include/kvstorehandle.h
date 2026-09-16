#pragma once

#include <filesystem>

// Seperate file.
class KVStoreHandle {
    private:
        uint64_t activeFileID;
        std::filesystem::path absDirPath;
        // hash table pointer.

    public:
        uint64_t getActiveFileID();
        void setActiveFileID(uint64_t newID);

        std::filesystem::path getAbsDirPath();
        void setAbsDirPath();
};