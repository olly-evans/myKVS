
#include "endian.h"

namespace Endian {

    static constexpr bool IS_LITTLE_ENDIAN = std::endian::native == std::endian::little;

    uint8_t htoleU8(uint8_t value) {
        return value; 
    }

    uint16_t htoleU16(uint16_t value) {
        if constexpr (IS_LITTLE_ENDIAN) {
            return value;
        } else {
            return std::byteswap(value);
        }
    }

    uint32_t htoleU32(uint32_t value) {
        if constexpr (IS_LITTLE_ENDIAN) {
            return value;
        } else {
            return std::byteswap(value);
        }
    }

    uint64_t htoleU64(uint64_t value) {
        if constexpr (IS_LITTLE_ENDIAN) {
            return value;
        } else {
            return std::byteswap(value);
        }
    }

}
