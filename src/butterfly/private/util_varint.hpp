/**
 * @file util_varint.hpp
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

#ifndef BUTTERFLY_UTIL_VARINT_HPP
#define BUTTERFLY_UTIL_VARINT_HPP

#include <butterfly/util_assert.hpp>
#include <butterfly/util_platform.hpp>

namespace butterfly {
    /** Reads an uint32_t, encoded by protobuf */
    force_inline uint32_t read_varint32(
        const char* data, uint8_t& read, std::size_t size = 0, std::size_t position = 0 ) {
        uint32_t result = 0; // result
        char tmp;            // holds last read value
        read = 0;            // make sure read is zeroed

        do {
            ASSERT_LESS( read, 5, "Trying to read more than 5 bytes for varint32" );

            tmp = data[position++];
            result |= ( uint32_t )( tmp & 0x7F ) << ( 7 * read );
            ++read;
        } while ( tmp & 0x80 );

        return result;
    }

    /** Read a varint from a buffer, fast version. */
    force_inline uint8_t* read_varint32_fast( uint8_t* buffer, uint32_t& value ) {
        /* clang-format off */
        uint8_t* ptr = buffer; // avoids indirection, indicates bytes read
        uint32_t b;
        uint32_t result = 0;

        ASSERT_TRUE(ptr, "Buffer is null");

        b = *(ptr++); result  = b      ; if (!(b & 0x80)) goto done;
        result -= 0x80;
        b = *(ptr++); result += b <<  7; if (!(b & 0x80)) goto done;
        result -= 0x80 << 7;
        b = *(ptr++); result += b << 14; if (!(b & 0x80)) goto done;
        result -= 0x80 << 14;
        b = *(ptr++); result += b << 21; if (!(b & 0x80)) goto done;
        result -= 0x80 << 21;
        b = *(ptr++); result += b << 28; if (!(b & 0x80)) goto done;

        // More than 5 bytes read, corrupt
        ASSERT_TRUE(0 != 0, "Trying to read more than 5 bytes for varint32");

        done:
            value = result;
            return ptr;
        /* clang-format on */
    }
} /* butterfly */

#endif /* BUTTERFLY_UTIL_VARINT_HPP */
