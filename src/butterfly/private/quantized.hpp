/**
 * @file quantized.hpp
 * @author Robin Dietrich <me (at) invokr (dot) org>
 *
 * @par License
 *    Butterfly Replay Parser
 *    Copyright 2014-2016 Robin Dietrich
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef BUTTERFLY_QUANTIZED_HPP
#define BUTTERFLY_QUANTIZED_HPP

#include <cmath>
#include <cstdint>

#include <butterfly/util_bitstream.hpp>
#include <butterfly/util_platform.hpp>

namespace butterfly {
    /** Decode quantized floats */
    class quantized_float_decoder {
    public:
        /** Different encoding flags */
        enum flags {
            QFE_NONE                    = 0,
            QFE_ROUNDDOWN               = ( 1 << 0 ),
            QFE_ROUNDUP                 = ( 1 << 1 ),
            QFE_ENCODE_ZERO_EXACTLY     = ( 1 << 2 ),
            QFE_ENCODE_INTEGERS_EXACTLY = ( 1 << 3 )
        };

        /** Constructor, initialized the decoder */
        quantized_float_decoder( uint8_t bc, uint8_t eflags, float min, float max )
            : min( min ), max( max ), high_low_mul( 0.0f ), decode_mul( 0.0f ), bc( bc ), flags( 0 ),
              encode_flags( eflags ), is_noscale( false ) {
            // force noscale decoding
            if ( bc == 0 || bc >= 32 ) {
                is_noscale = true;
                return;
            }

            validate_flags();
            uint32_t steps = ( 1 << bc );

            if ( flags & QFE_ROUNDDOWN ) {
                float range = max - min;
                max -= ( range / steps );
            } else if ( flags & QFE_ROUNDUP ) {
                float range = max - min;
                min += ( range / steps );
            }

            if ( flags & QFE_ENCODE_INTEGERS_EXACTLY ) {
                int32_t iMin   = static_cast<int32_t>( min );
                int32_t iMax   = static_cast<int32_t>( max );
                int32_t iDelta = ( iMax - iMin ) < 1 ? 1 : ( iMax - iMin );

                int32_t iLog2 = ceil( log2( iDelta ) ) + 1;
                int32_t range = ( 1 << iLog2 );

                uint32_t iBits = bc;
                while ( ( 1 << iBits ) < range ) {
                    ++iBits;
                }

                if ( iBits > bc ) {
                    bc    = iBits;
                    steps = ( 1 << bc );
                }

                max = min + range - ( static_cast<float>( ( 1 << iDelta ) ) / static_cast<float>( steps ) );
            }

            assign_multipliers( steps );

            // Remove unessecary flags
            if ( flags & QFE_ROUNDDOWN ) {
                if ( quantize( min ) == min )
                    flags &= ~QFE_ROUNDDOWN;
            }

            if ( flags & QFE_ROUNDUP ) {
                if ( quantize( max ) == max )
                    flags &= ~QFE_ROUNDUP;
            }

            if ( flags & QFE_ENCODE_ZERO_EXACTLY ) {
                if ( quantize( 0.0f ) == 0.0f )
                    flags &= ~QFE_ENCODE_ZERO_EXACTLY;
            }
        }

        /** Default destructor */
        ~quantized_float_decoder() = default;

        /** Return whether to decode this as noscale */
        force_inline bool noscale() { return is_noscale; }

        /** Set optimal decoding flags based on min / max value and encoding flags */
        void validate_flags() {
            flags = encode_flags;

            if ( ( min == 0.0f && flags & QFE_ROUNDDOWN ) || ( max == 0.0f && flags & QFE_ROUNDUP ) ) {
                flags &= ~QFE_ENCODE_ZERO_EXACTLY;
            }

            if ( min == 0.0f && flags & QFE_ENCODE_ZERO_EXACTLY ) {
                flags |= QFE_ROUNDDOWN;
                flags &= ~QFE_ENCODE_ZERO_EXACTLY;
            }

            if ( max == 0.0f && flags & QFE_ENCODE_ZERO_EXACTLY ) {
                flags |= QFE_ROUNDUP;
                flags &= ~QFE_ENCODE_ZERO_EXACTLY;
            }

            bool needToTestZero = ( min < 0.0f && max > 0.0f );
            if ( !needToTestZero ) {
                flags &= ~QFE_ENCODE_ZERO_EXACTLY;
            }

            if ( flags & QFE_ENCODE_INTEGERS_EXACTLY ) {
                flags &= ~( QFE_ROUNDUP | QFE_ROUNDDOWN | QFE_ENCODE_ZERO_EXACTLY );
            }
        }

        /** Assign multipliers */
        force_inline void assign_multipliers( uint32_t steps ) {
            float range      = max - min;
            uint32_t highVal = bc == 32 ? 0xFFFFFFFE : ( ( 1 << bc ) - 1 );
            high_low_mul =
                std::abs( range ) <= 0.0 ? static_cast<float>( highVal ) : static_cast<float>( highVal ) / range;

            if ( ( uint32_t )( high_low_mul * range ) > highVal || ( high_low_mul * range ) > (double)highVal ) {
                float multipliers[] = {0.9999f, 0.99f, 0.9f, 0.8f, 0.7f};
                for ( auto mul : multipliers ) {
                    high_low_mul = static_cast<float>( highVal / range ) * mul;
                    if ( ( uint32_t )( high_low_mul * range ) > highVal || ( high_low_mul * range ) > (double)highVal )
                        continue;
                    break;
                }
            }

            decode_mul = 1.0f / ( steps - 1 );
        }

        /** Quantize a flot */
        float quantize( float f ) {
            if ( f < min ) {
                return min;
            } else if ( f > max ) {
                return max;
            }

            uint32_t i = static_cast<uint32_t>( ( f - min ) * high_low_mul );
            return min + ( max - min ) * ( (float)(i)*decode_mul );
        }

        /** Decode the float */
        float decode( bitstream& b ) {
            if ( flags & QFE_ROUNDDOWN && b.readBool() )
                return min;

            if ( flags & QFE_ROUNDUP && b.readBool() )
                return max;

            if ( flags & QFE_ENCODE_ZERO_EXACTLY && b.readBool() )
                return 0.0f;

            uint32_t u = b.read( bc );
            return min + ( max - min ) * ( static_cast<float>( u ) * decode_mul );
        }

    private:
        /** Minimum */
        float min;
        /** Maximum */
        float max;
        /** High / Low multiplicator */
        float high_low_mul;
        /** Quantified multiplicator */
        float decode_mul;
        /** Bitcount unless truncated */
        uint8_t bc;
        /** Decoding flags */
        uint8_t flags : 4;
        /** Networked flags */
        uint8_t encode_flags : 4;
        /** Whether to decode as noscale */
        bool is_noscale;
    };
} /* butterfly */

#endif /* BUTTERFLY_QUANTIZED_HPP */
