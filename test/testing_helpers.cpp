#include "testing_helpers.h"

fs::path createTempTestDir(std::string dirName) {
    fs::path src = TEST_ROOT;
    fs::path path = src / dirName;
    fs::create_directories(path);
    return path;
}