#pragma once

#include <stdint.h>
#include <expected>

enum class KVErrorCode : uint8_t {
    StoreNotOpen = 1,
    ProvidedPathNotAbsolute,
    NoActiveDatafilePath,
    OutputFilestreamBad,
    InputFilestreamBad,
    KeyNotFound,
    ReadPathDoesNotExist,
    ReadExceedsFileSize,
    MismatchedCRC
};

struct KVError {
    KVErrorCode code;
    std::string message;
};

using KVExpected = std::expected<KVStoreHandle, KVError>;
using KVResult = std::expected<std::string, KVError>;