#include "testing_helpers.h"
#include "detect_store.h"

namespace TestHelpers {

    fs::path create_tmp_dir(std::string dirname) {
        fs::path src = fs::temp_directory_path();
        fs::path path = src / dirname;
        fs::create_directories(path);
        return path;
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
