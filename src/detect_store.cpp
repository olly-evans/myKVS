
#include "detect_store.h"

namespace DetectStore {

    bool isNumericStem(const std::string& stem) {
        return !stem.empty() && std::all_of(stem.begin(), stem.end(), ::isdigit);
    }

    bool isValidStoreFile(const fs::directory_entry& entry) {

        // all storefiles must contain two periods.


        // either .lock .hint or .data ext.

        // can be more specific with .lock or .hint later when we use them.

        // .data must have numeric stem.

        return true;
    }

    bool allValidStoreFiles(const fs::path& dir) {

        for (const auto& candidateFile : fs::directory_iterator(dir)) {
            if (!isValidStoreFile(candidateFile))
                return false;
        }
        return true;
    }

    bool isStore(const fs::path& dir) {

        // std::vector<fs::path> candidates = DetectStore::listCandidateDatafiles(dir);

        // if (candidates.empty())
        //     return false;
        

        // stH.setDatafileExt(DetectStore::extractExtension(candidates.front()));
        return true;
    }

} // DetectStore namespace