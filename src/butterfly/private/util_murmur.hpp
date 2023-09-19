/**
 * @file util_murmur.hpp
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

#ifndef BUTTERFLY_UTIL_MURMUR_HPP
#define BUTTERFLY_UTIL_MURMUR_HPP

#include <cstdint>

namespace butterfly {
    /** MurmurHash implementation from https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp */
    uint64_t MurmurHash64( const void* key, uint32_t len, uint64_t seed ) {
        const uint32_t m = 0x5bd1e995;
        const int r      = 24;

        uint32_t h1 = uint32_t( seed ) ^ len;
        uint32_t h2 = uint32_t( seed >> 32 );

        const uint32_t* data = (const uint32_t*)key;

        while ( len >= 8 ) {
            uint32_t k1 = *data++;
            k1 *= m;
            k1 ^= k1 >> r;
            k1 *= m;
            h1 *= m;
            h1 ^= k1;
            len -= 4;

            uint32_t k2 = *data++;
            k2 *= m;
            k2 ^= k2 >> r;
            k2 *= m;
            h2 *= m;
            h2 ^= k2;
            len -= 4;
        }

        if ( len >= 4 ) {
            uint32_t k1 = *data++;
            k1 *= m;
            k1 ^= k1 >> r;
            k1 *= m;
            h1 *= m;
            h1 ^= k1;
            len -= 4;
        }

        switch ( len ) {
        case 3:
            h2 ^= ( (unsigned char*)data )[2] << 16;
        case 2:
            h2 ^= ( (unsigned char*)data )[1] << 8;
        case 1:
            h2 ^= ( (unsigned char*)data )[0];
            h2 *= m;
        };

        h1 ^= h2 >> 18;
        h1 *= m;
        h2 ^= h1 >> 22;
        h2 *= m;
        h1 ^= h2 >> 17;
        h1 *= m;
        h2 ^= h1 >> 19;
        h2 *= m;

        uint64_t h = h1;

        h = ( h << 32 ) | h2;

        return h;
    }
}

#endif /* BUTTERFLY_UTIL_MURMUR_HPP */
