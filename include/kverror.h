#pragma once

#include <stdint.h>
#include <expected>

// class KVStoreHandle;
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

using KVResult = std::expected<KVStoreHandle, KVError>;