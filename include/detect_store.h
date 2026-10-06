#pragma once

#include "kvstorehandle.h"

#include <string>
#include <algorithm>
#include <array>

namespace fs = std::filesystem;

namespace DetectStore {

    std::string getDatafileStatus(DatafileStatus status);

    bool isAppendOnlyDatafile(fs::path datafile);

    bool isNumericStem(const std::string& stem);

    bool isValidStoreExtension(const std::string& fileExt);
    bool isValidStoreFile(const fs::path& filepath);
    bool hasOnlyValidStoreFiles(const fs::path& dir);

    bool isStore(const fs::path& dir);

}