/**
 * @file 02-spew-types.cpp
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

/**
 * You can generate a list of unique types and their number of occurrences like this:
 * ./02-spew-types [replay] | sort | uniq -c | sort -r
 */

#include <string>
#include <cstdio>
#include <cstdint>

#include <butterfly/proto/demo.pb.h>
#include <butterfly/proto/netmessages.pb.h>
#include <butterfly/butterfly.hpp>
#include <butterfly/util_assert.hpp>

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

int spew_types(butterfly::dem_packet &p) {
    CDemoSendTables proto;
    ASSERT_TRUE( proto.ParseFromArray( p.data, p.size ), "Unable to parse protobuf packet" );

    // Packet contents: Size as varint, Serialized Flattables buffer
    const std::string& buf = proto.data();
    ASSERT_GREATER( buf.size(), 10, "Sendtables packet corrupt" );

    // Read size and verify that we have enough bytes remaining
    uint32_t size, psize;
    uint8_t* data = read_varint32_fast( (uint8_t*)buf.c_str(), size );
    psize         = data - (uint8_t*)buf.c_str();

    ASSERT_GREATER( buf.size() - psize, size, "Sendtables packet corrupt" );

    CSVCMsg_FlattenedSerializer serializers;
    ASSERT_TRUE( serializers.ParseFromArray( data, size ), "Unable to parse buffer as FlattenedSerializer packet" );

    for (auto &ser : serializers.serializers()) {
        for (auto &f : ser.fields_index()) {
            std::string type = serializers.symbols(serializers.fields(f).var_type_sym());
            printf("%s\n", type.c_str());
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: 02-spit-types <replay>\n");
        return 1;
    }

    butterfly::demfile file(argv[1]);

    while (file.good()) {
        auto p = file.get();

        switch ( p.type ) {
        case DEM_SendTables:
            return spew_types(p);
        case DEM_Stop:
            printf("Error: Reached EOF before SendTables packet.\n");
            break;
        }
    }
}
