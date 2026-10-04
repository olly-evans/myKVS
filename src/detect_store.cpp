
#include "detect_store.h"

namespace DetectStore {

    bool isNumericStem(const std::string& stem) {
        return !stem.empty() && std::all_of(stem.begin(), stem.end(), ::isdigit);
    }

    bool isValidStoreExtension(const std::string& fileExt) {
        
        std::array<std::string, 3> validExtensions = {".data", ".hint", ".lock"};

        for (const auto& validExt : validExtensions) {
            if ((validExt == fileExt))
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

        if (!isValidStoreExtension(filepath.extension()))
            return false;

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
        
        bool dirExists = fs::exists(dir);
        bool allValidStoreFiles = hasOnlyValidStoreFiles(dir);

        // More criteria if required.

        return allValidStoreFiles && dirExists;
    }

} // DetectStore namespace