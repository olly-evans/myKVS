#include "testing_helpers.h"

fs::path createTempTestDir(std::string dirName) {
    fs::path src = TEST_ROOT;
    std::string testDir = "test/" + dirName;
    fs::path path = src / testDir;
    fs::create_directories(path);
    return path;
}