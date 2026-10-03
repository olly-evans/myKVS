#pragma once

#include "kvstorehandle.h"

#include <string>
#include <algorithm>
#include <array>

namespace fs = std::filesystem;

namespace DetectStore {

    bool isNumericStem(const std::string& stem);

    bool isValidStoreFile(const fs::directory_entry& entry);
    bool allValidStoreFiles(const fs::path& dir);

    bool isStore(const fs::path& dir);

}