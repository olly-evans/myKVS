#include <string>
#include <stdint.h>

enum class KVErrorCode : uint8_t {
    StoreNotOpen = 1,
    NoActiveDatafilePath,
    OutputFilestreamBad,
    InputFilestreamBad,
};

struct KVError {
    KVErrorCode code;
    std::string message;
};