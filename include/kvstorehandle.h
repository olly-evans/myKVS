#pragma once

#include <filesystem>

class KVStoreHandle {
    private:
        uint32_t activeFileID;
        std::filesystem::path absDirPath;
        std::string datafileExtension;
        // hash table pointer.

    public:
        // KVStoreHandle(mutex.lock());
        // ~KVStoreHandle();

        [[nodiscard]] uint32_t getActiveFileID();
        void setActiveFileID(std::filesystem::path dataDir);

        [[nodiscard]] std::filesystem::path getAbsDirPath();
        void setAbsDirPath();

        [[nodiscard]] std::string getDatafileExt();
        void setDatafileExt(std::string fileExtension);

};