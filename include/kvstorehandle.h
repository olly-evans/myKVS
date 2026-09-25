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

        [[nodiscard]] uint32_t getActiveFileID() const;
        void updateActiveFileID(std::filesystem::path dataDir);

        [[nodiscard]] std::filesystem::path getAbsDirPath() const;
        void setAbsDirPath();

        [[nodiscard]] std::string getDatafileExt() const;
        void setDatafileExt(std::string fileExtension);

};