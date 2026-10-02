#include "testing_helpers.h"

fs::path createTempTestDir(std::string dirname) {
    fs::path src = TEST_ROOT;
    fs::path path = src / dirname;
    fs::create_directories(path);
    return path;
}