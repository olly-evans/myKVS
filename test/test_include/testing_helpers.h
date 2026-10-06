#pragma once

#include "kvstore.h"

struct TempDirGuard {

    fs::path dir;

    ~TempDirGuard() { 
        std::error_code ec;
        fs::remove_all(dir, ec);
    }
};

struct HandleTestAccess {
    static auto& keyDir(KVStoreHandle& stH) { return stH.keyDir; }
};

fs::path test_create_tmp_dir(std::string dirname);

fs::path test_create_append_only_datafile_path(const fs::path tmpDir, const std::string ext, const uint32_t id);
fs::path test_create_read_only_datafile_path(const fs::path tmpDir, const std::string ext, const uint32_t id);

void test_create_mock_aol_file(const fs::path tmpDir, const std::string ext, const uint32_t id);
void test_create_mock_rol_file(const fs::path tmpDir, const std::string ext, const uint32_t id);

void test_create_n_mock_files(const fs::path tmpDir, const std::string ext, const uint32_t startID, const uint32_t n);

KVResult test_simulate_put(fs::path tmpDir); // seperate testing_put file perhaps.