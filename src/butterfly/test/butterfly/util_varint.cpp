/**
 * @file util_varint.cpp
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

#include <catch2/catch_test_macros.hpp>
#include "../../private/util_varint.hpp"

using namespace butterfly;

TEST_CASE( "varint", "[util_varint.hpp]" ) {
    uint8_t read;
    unsigned char c[] = {0x9E, 0xA7, 0x05};
    const char* c2    = reinterpret_cast<const char*>( c );

    // Normal
    REQUIRE( read_varint32( c2, read, 3 ) == 86942 );
    REQUIRE( read == 3 );

    // Fast version
    uint32_t ret = 0;
    read_varint32_fast( c, ret );
    REQUIRE( ret == 86942 );
}
