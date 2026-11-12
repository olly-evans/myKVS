
#include "endian.h"

namespace Endian {

    uint8_t htoleU8(uint8_t value) {
        return value; 
    }

    uint16_t htoleU16(uint16_t value) {
        if constexpr (std::endian::native == std::endian::little) {
            return value;
        } else {
            return std::byteswap(value);
        }
    }

}
