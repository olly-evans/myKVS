#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

namespace DetectStore {
    bool isNumericStem(const std::string& stem);
    bool isCandidateDatafile(const fs::directory_entry& entry);
    std::vector<fs::path> listCandidateDatafiles(const fs::path& dir);
    std::string extractExtension(const fs::path& datafilePath);
}
