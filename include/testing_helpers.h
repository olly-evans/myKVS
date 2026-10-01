#pragma once

#include "kvstore.h"

#include <filesystem>

namespace fs = std::filesystem;

struct TempDirCleanup {
    fs::path dir;
    ~TempDirCleanup() { 
        std::error_code ec;
        fs::remove_all(dir, ec); 
    }
};

struct KVStoreTestAccess {
    static auto& keyDir(KVStore& kvs) { return kvs.keyDir; }
};

fs::path createTempTestDir(std::string dirName);