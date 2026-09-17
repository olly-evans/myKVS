#pragma once

#include <filesystem>

// Seperate file.
class KVStoreHandle {
    private:
        uint32_t activeFileID;
        std::filesystem::path absDirPath;
        // hash table pointer.

    public:
        uint64_t getActiveFileID();

        /* Set the ID of the active data file in the data directory */

        void setActiveFileID(std::filesystem::path dataDir);

        std::filesystem::path getAbsDirPath();
        void setAbsDirPath();
};