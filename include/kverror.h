#pragma once

#include <stdint.h>
#include <expected>

enum class KVErrorCode : uint8_t {
    StoreNotOpen = 1,
    ProvidedPathNotAbsolute,
    NoActiveDatafilePath,
    OutputFilestreamBad,
    InputFilestreamBad,
};

struct KVError {
    KVErrorCode code;
    std::string message;
};

using KVExpected = std::expected<KVStoreHandle, KVError>;
using KVResult = std::expected<void, KVError>;