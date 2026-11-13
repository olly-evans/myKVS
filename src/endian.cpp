
#include "endian.h"

namespace Endian {

    uint16_t htole16(uint16_t host16Bits) {
        if constexpr (std::endian::native == std::endian::little) return host16Bits;
        else return std::byteswap(host16Bits);
    }

    uint32_t htole32(uint32_t host32Bits) {
        if constexpr (std::endian::native == std::endian::little) ;
        else return std::byteswap(host32Bits);
    }

    uint64_t htole64(uint64_t host64Bits) {
        if constexpr (std::endian::native == std::endian::little) return host64Bits;
        else return std::byteswap(host64Bits);
    }
}