/**
 * @file dem.cpp
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

#include <cstddef>
#include <snappy.h>

#include <butterfly/proto/demo.pb.h>
#include <butterfly/dem.hpp>
#include <butterfly/util_assert.hpp>

#include "util_varint.hpp"

namespace butterfly {
    void dem_uncompress( dem_packet& msg, char* buffer, size_t buffer_size ) {
        size_t size_uncompressed = 0;

        const char* data = msg.data;
        msg.data         = nullptr;

        ASSERT_TRUE( snappy::IsValidCompressedBuffer( data, msg.size ), "Invalid snappy compression buffer" );
        ASSERT_TRUE(
            snappy::GetUncompressedLength( data, msg.size, &size_uncompressed ), "Unable to get uncompressed length" );
        ASSERT_GREATER_EQ( buffer_size, size_uncompressed, "Can't fit uncompressed data into Buffer" );
        ASSERT_TRUE( snappy::RawUncompress( data, msg.size, buffer ), "Failed to decompress data" );

        msg.type = msg.type & ~DEM_IsCompressed;
        msg.data = buffer;
        msg.size = size_uncompressed;
    }

    size_t dem_from_buffer( dem_packet& msg, char* buffer, size_t buffer_size, bool read_tick ) {
        if ( expect( buffer_size > 15, 1 ) ) { // fast version
            uint8_t* data = (uint8_t*)buffer;

            data = read_varint32_fast( data, msg.type );

            if ( read_tick )
                data = read_varint32_fast( data, (uint32_t&)msg.tick );

            data = read_varint32_fast( data, msg.size );

            msg.data = (char*)data;
            return ( msg.data - buffer ) + msg.size;
        } else { // slow version
            uint8_t r;
            char* buf = buffer;
            msg.type  = read_varint32( buf, r, buffer_size );
            buf += r;

            if ( read_tick )
                msg.tick = read_varint32( buf, r, buffer_size );
            buf += r;

            msg.size = read_varint32( buf, r, buffer_size );
            buf += r;
            msg.data = buf;

            return ( buf - buffer ) + msg.size;
        }
    }
} /* butterfly */
