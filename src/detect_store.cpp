
#include "detect_store.h"

namespace DetectStore {

    std::string getDatafileStatus(DatafileStatus status) {

        switch (status) {
            case DatafileStatus::Active:   
                return ".aol";
            case DatafileStatus::ReadOnly:
                return ".rol";
            default:
                throw std::runtime_error("Uknown datafile status.");
        }
    }

    bool isAppendOnlyDatafile(fs::path datafile) {
        fs::path datafileStatus = datafile.stem().extension().string();
        return getDatafileStatus(DatafileStatus::Active) == datafileStatus;
    }

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

        if (fs::is_empty(dir))
            return false;

        bool foundAnAppendOnlyDatafile = false;

        for (const auto& currentFile : fs::directory_iterator(dir)) {
            
            if (!currentFile.is_regular_file())
                return false;
            if (!isValidStoreFile(currentFile))
                return false;
            
            if (foundAnAppendOnlyDatafile && isAppendOnlyDatafile(currentFile.path()))
                return false;
            foundAnAppendOnlyDatafile = isAppendOnlyDatafile(currentFile.path());            
        }
        return true;
    }

    bool isStore(const fs::path& dir) {
        
        std::error_code ec;

        bool pathExists = fs::exists(dir, ec);
        bool pathIsDir = fs::is_directory(dir, ec);
        bool allValidStoreFiles = hasOnlyValidStoreFiles(dir);

        // More criteria if required.

        bool isStore = pathExists && pathIsDir && allValidStoreFiles;

        return isStore;
    }

} // DetectStore namespace