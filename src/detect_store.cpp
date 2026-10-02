
#include "detect_store.h"

namespace DetectStore {

    bool isNumericStem(const std::string& stem) {
        return !stem.empty() && std::all_of(stem.begin(), stem.end(), ::isdigit);
    }

    bool isCandidateDatafile(const fs::directory_entry& entry) {
        if (!entry.is_regular_file()) {
            return false;
        }
        std::string stem = entry.path().stem().string();
        return isNumericStem(stem);
    }

    std::vector<fs::path> listCandidateDatafiles(const fs::path& dir) {
        std::vector<fs::path> candidates;
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (isCandidateDatafile(entry)) {
                candidates.push_back(entry.path());
            }
        }
        return candidates;
    }

    std::string extractExtension(const fs::path& datafilePath) {
        return datafilePath.extension().extension().string();
    }

    bool isStore(KVStoreHandle& stH, const fs::path& dir) {

        std::vector<fs::path> candidates = DetectStore::listCandidateDatafiles(dir);

        if (candidates.empty())
            return false;
        

        stH.setDatafileExt(DetectStore::extractExtension(candidates.front()));
        return true;
    }
} // DetectStore namespace