
#include "detect_store.h"

namespace DetectStore {

    bool isNumericStem(const std::string& stem) {
        return !stem.empty() && std::all_of(stem.begin(), stem.end(), ::isdigit);
    }

    bool isValidStoreFile(const fs::path& filepath) {

        std::array<std::string, 3> validExtensions = {".data", ".hint", ".lock"};

        if (filepath.empty())
            return false;

        std::string filename = filepath.filename().string(); // "0.aol.data i.e"
        std::string stem = filepath.filename().stem().stem();

        if (!isNumericStem(stem))
            return false;

        // If using stem() twice returns as using it once too then only one period in filename and invalid file.
        if (stem == filepath.filename().stem())
            return false;

        // either .lock .hint or .data ext.

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