#pragma once

#include "kvstore.h"

struct TempDirGuard {
    fs::path dir;
    ~TempDirGuard() { 
        std::error_code ec;
        fs::remove_all(dir, ec); 
    }
};

// NAME
struct KVStoreTestAccess {
    static auto& keyDir(KVStoreHandle& stH) { return stH.keyDir; }
};

fs::path createTempTestDir(std::string dirName);