#include "testing_helpers.h"

// Namespace perhaps.

fs::path test_create_tmp_dir(std::string dirname) {
    fs::path src = TEST_ROOT;
    fs::path path = src / dirname;
    fs::create_directories(path);
    return path;
}

fs::path test_create_append_only_datafile_path(const fs::path tmpDir, const std::string ext, const uint32_t id) {
    return tmpDir / (std::to_string(id) + ".aol" + ext);
}

fs::path test_create_read_only_datafile_path(const fs::path tmpDir, const std::string ext, const uint32_t id) {
    return tmpDir / (std::to_string(id) + ".rol" + ext);
}

void test_create_mock_file(const fs::path tmpDir, const std::string ext, const uint32_t id) {

    fs::path file = test_create_append_only_datafile_path(tmpDir, ext, id);
    std::ofstream out(file, std::ios::app | std::ios::binary);
    if (!fs::exists(file))
        std::cout << "Mock file could not created" << "\n";
    out.close();

}

void test_create_n_mock_files(fs::path tmpDir, std::string ext, const uint32_t startID, const uint32_t n) {
    
    for (uint32_t i = startID; i <= n; i++) {
        test_create_mock_file(tmpDir, ext, i);
    }
}

// KVResult test_simulate_put(fs::path tmpDir) {
    
//     // test_create_n_mock_files(tmpDir, 1, 3);
// }
