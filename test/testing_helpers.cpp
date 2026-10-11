#include "testing_helpers.h"
#include "detect_store.h"

namespace TestHelpers {

    // put in constructor of tmp dir struct.
    fs::path create_tmp_dir(const std::source_location loc) {
        
        /* Creates a fresh, empty temporary directory for one test and returns its path.
        The directory is named <source filename>_<n> (e.g. "test_put_2") and lives under the
        system temp directory. */

        static uint16_t counter = 0;

        fs::path filename = static_cast<fs::path>(loc.file_name());
        std::string src = filename.stem().string(); // test_put

        fs::path tmp = fs::temp_directory_path();
        fs::path dir = tmp / (src + "_" + std::to_string(counter));
        fs::create_directories(dir);

        if (fs::exists(dir))
            counter++;

        return dir;
    }

    fs::path create_append_only_datafile_path(const fs::path tmpDir, const std::string ext, const uint32_t id) {
        return tmpDir / (std::to_string(id) + DetectStore::getDatafileStatus(DFStatus::Active) + ext);
    }

    fs::path create_read_only_datafile_path(const fs::path tmpDir, const std::string ext, const uint32_t id) {
        return tmpDir / (std::to_string(id) + ".rol" + ext);
    }

    void create_mock_aol_file(const fs::path tmpDir, const std::string ext, const uint32_t id) {

        fs::path file = create_append_only_datafile_path(tmpDir, ext, id);
        std::ofstream out(file, std::ios::app | std::ios::binary);
        if (!fs::exists(file))
            std::cout << "Mock append-only file could not created" << "\n";
        out.close();
    }

    void create_mock_rol_file(const fs::path tmpDir, const std::string ext, const uint32_t id) {

        fs::path file = create_read_only_datafile_path(tmpDir, ext, id);
        std::ofstream out(file, std::ios::app | std::ios::binary);
        if (!fs::exists(file))
            std::cout << "Mock read-only file could not created" << "\n";
        out.close();
    }

    void create_n_mock_files(fs::path tmpDir, std::string ext, const uint32_t startID, const uint32_t n) {
        
        for (uint32_t i = startID; i <= n; i++) {
            create_mock_aol_file(tmpDir, ext, i);
        }
    }

} // Test

// KVResult test_simulate_put(fs::path tmpDir) {
    
//     // test_create_n_mock_files(tmpDir, 1, 3);
// }
