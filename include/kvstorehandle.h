#pragma once

#include <filesystem>

class KVStoreHandle {
    private:
        uint32_t activeFileID;
        size_t maxDatafileBytes;

        std::filesystem::path absDirPath;
        std::string datafileExtension;

        bool readWrite;
        // hash table pointer.

    public:
        // KVStoreHandle(mutex.lock());
        // ~KVStoreHandle();

        [[nodiscard]] uint32_t getActiveFileID() const;
        void updateActiveDatafileID(std::filesystem::path dataDir);

        [[nodiscard]] size_t getMaxDatafileBytes() const;
        void setMaxDatafileBytes(size_t maxBytes);

        [[nodiscard]] std::filesystem::path getAbsDirPath() const;
        void setAbsDirPath();

        [[nodiscard]] std::string getDatafileExt() const;
        void setDatafileExt(std::string fileExtension);

        [[nodiscard]] bool getReadWrite() const;
        void setReadWrite(bool readWrite);

};