#pragma once

#include "kvstore.h"

struct TempDirGuard {
    fs::path dir;
    ~TempDirGuard() { 
        std::error_code ec;
        fs::remove_all(dir, ec); 
    }
};

struct KVStoreTestAccess {
    static auto& keyDir(KVStore& kvs) { return kvs.keyDir; }
};

fs::path createTempTestDir(std::string dirName);