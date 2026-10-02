#pragma once

#include "kvstorehandle.h"

#include <vector>
#include <string>
#include <algorithm>

namespace fs = std::filesystem;

namespace DetectStore {
    bool isNumericStem(const std::string& stem);

    bool isCandidateDatafile(const fs::directory_entry& entry);
    bool allValidStoreFiles(const fs::path& dir);

    bool isStore(const fs::path& dir);

    // run is candidatedatafile on all files in dir, if stem of .data is numeric
    // if .lock or .hint also true.
    // anything else and disqualified for now.
}