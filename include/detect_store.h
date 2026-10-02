#pragma once

#include "kvstorehandle.h"

#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

namespace DetectStore {
    bool isNumericStem(const std::string& stem);

    bool isCandidateDatafile(const fs::directory_entry& entry);
    std::vector<fs::path> listCandidateDatafiles(const fs::path& dir);

    std::string extractExtension(const fs::path& datafilePath);
    
    bool isStore(const fs::path& dir);
}