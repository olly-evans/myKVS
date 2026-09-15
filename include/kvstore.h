#pragma once

#include <string>
#include <optional>

struct Error {
    int val{0};
    std::string description;
};

struct KVStoreHandle {
    int activeFileID{-1};
    std::string absDirPath;
};

class KVStore {

    public:
        KVStore();
        ~KVStore();


    private:
        std::optional<KVStoreHandle> open();
        std::optional<KVStoreHandle> openReadOnly();

};
