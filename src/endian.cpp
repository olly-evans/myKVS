
#include "endian.h"

namespace Endian {

    uint8_t htoleU8(uint8_t byte) {
        if constexpr (std::endian::native == std::endian::little) {
                return byte;
        } else {
            return std::byteswap(byte);
        }    
    }
    

}
