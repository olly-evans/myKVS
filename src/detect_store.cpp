
#include "detect_store.h"

namespace DetectStore {

    bool isNumericStem(const std::string& stem) {
        return !stem.empty() && std::all_of(stem.begin(), stem.end(), ::isdigit);
    }

    bool isValidStoreExtension(const std::string& fileExt) {
        
        std::array<std::string, 3> validExtensions = {".data", ".hint", ".lock"};

        for (const auto& validExt : validExtensions) {
            if ((fileExt == validExt))
                return true;
        }
        return false;
    }

    bool isValidStoreFile(const fs::path& filepath) {


        if (filepath.empty())
            return false;

        std::string stem = filepath.filename().stem().stem(); // "0.aol.data" -> "0", "0.data" -> 0

        if (!isNumericStem(stem))
            return false;

        // "0.aol.data" -> "0" also "0.data" -> 0
        if (stem == filepath.filename().stem())
            return false;

        // either .lock .hint or .data ext.

        if (!isValidStoreExtension(filepath))
            return false;

        // can be more specific with .lock or .hint later when we use them.

        // .data must have numeric stem.

        return true;
    }

    bool hasOnlyValidStoreFiles(const fs::path& dir) {

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