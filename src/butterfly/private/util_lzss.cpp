#include "util_lzss.hpp"

namespace butterfly {
    constexpr uint32_t LZSS_ID =
        ( ( 'S' << 24 ) | ( 'S' << 16 ) | ( 'Z' << 8 ) | ( 'L' ) );
    size_t LZSS_GetActualSize( const uint8_t* input, size_t input_size ) {
        if ( input_size < 8 ||
             reinterpret_cast<const uint32_t*>( input )[0] != LZSS_ID )
            return 0;
        return reinterpret_cast<const uint32_t*>( input )[1];
    }

    size_t LZSS_Uncompress( const uint8_t* input, size_t input_size,
                            uint8_t* output, size_t output_capacity ) {
        if ( input_size < 8 ||
             reinterpret_cast<const uint32_t*>( input )[0] != LZSS_ID ||
             reinterpret_cast<const uint32_t*>( input )[1] > output_capacity )
            return 0;
        input += 8;
        input_size -= 8;

        uint8_t cmd_byte     = 0;
        uint8_t get_cmd_byte = 0;
        size_t current_off   = 0;
        while ( true ) {
            if ( get_cmd_byte == 0 ) {
                if ( input_size < 1 )
                    break;
                input_size--;
                cmd_byte = *input++;
            }
            get_cmd_byte = ( get_cmd_byte + 1 ) & 7;

            if ( cmd_byte & 1 ) {
                if ( input_size < 2 )
                    break;
                input_size -= 2;
                const uint8_t cmd_part1 = *input++, cmd_part2 = *input++;
                const uint16_t position =
                    ( static_cast<uint16_t>( cmd_part1 ) << 4 ) |
                    ( cmd_part2 >> 4 );
                const uint8_t count = ( cmd_part2 & 0x0F ) + 1;
                if ( count == 1 || current_off <= position ||
                     current_off + count > output_capacity )
                    break;
                for ( uint8_t i = 0; i < count; i++ )
                    output[current_off + i] =
                        output[current_off + i - position - 1];
                current_off += count;
            } else if ( current_off < output_capacity && input_size > 0 ) {
                input_size--;
                output[current_off++] = *input++;
            } else
                break;
            cmd_byte >>= 1;
        }
        return current_off;
    }
}  // namespace butterfly
