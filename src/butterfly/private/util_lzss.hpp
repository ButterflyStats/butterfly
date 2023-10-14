#ifndef BUTTERFLY_UTIL_LZSS_HPP
#define BUTTERFLY_UTIL_LZSS_HPP

#include <stddef.h>
#include <cstdint>

namespace butterfly {
    size_t LZSS_GetActualSize( const uint8_t* input, size_t input_size );
    size_t LZSS_Uncompress( const uint8_t* input, size_t input_size,
                            uint8_t* output, size_t output_capacity );
}  // namespace butterfly

#endif /* BUTTERFLY_UTIL_LZSS_HPP */
