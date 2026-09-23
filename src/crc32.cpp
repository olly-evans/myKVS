#include <string>
#include <cstdint>
#include <array>

#include "crc32.h"

namespace {
    std::array<uint32_t, 256> makeCrcTable() {
        std::array<uint32_t, 256> table{};
        for (uint32_t i = 0; i < 256; ++i) {
            uint32_t c = i; // 0-255
            for (int k = 0; k < 8; ++k)
                c = (c & 1) ? (CRC32_POLY ^ (c >> 1)) : (c >> 1);
            table[i] = c;
        }
        return table;
    }

    const std::array<uint32_t, 256>& crcTable() {
        static const auto table = makeCrcTable();
        return table;
    }
}

void crcUpdate(uint32_t& crc, const void* data, size_t len) {
    const auto& table = crcTable();
    auto bytes = static_cast<const unsigned char*>(data);
    for (size_t i = 0; i < len; ++i)
        crc = table[(crc ^ bytes[i]) & 0xFF] ^ (crc >> 8);
}