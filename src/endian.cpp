
#include "endian.h"

namespace Endian {

    uint16_t htoleU16(uint16_t host16Bits) {
        if constexpr (std::endian::native == std::endian::little) return std::byteswap(host16Bits);
        else return host16Bits;
    }

    uint32_t htoleU32(uint32_t host32Bits) {
        if constexpr (std::endian::native == std::endian::little) return std::byteswap(host32Bits);
        else return host32Bits;
    }

    uint64_t htoleU64(uint64_t host64Bits) {
        if constexpr (std::endian::native == std::endian::little) return std::byteswap(host64Bits);
        else return host64Bits;
    }
}