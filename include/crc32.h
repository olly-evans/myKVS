#pragma once

#include <cstdint>
#include <cstddef>

// Feeds `len` bytes starting at `data` through the running CRC-32
// accumulator `crc`. Call it once per field, in a fixed order,
// starting with crc = 0xFFFFFFFF and finishing with crc ^= 0xFFFFFFFF.
void crcUpdate(uint32_t& crc, const void* data, size_t len);