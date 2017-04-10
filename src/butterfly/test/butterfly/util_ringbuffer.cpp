/**
 * @file util_ringbuffer.cpp
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

#include <catch.hpp>
#include <string>
#include <butterfly/util_ringbuffer.hpp>

TEST_CASE( "ringbuffer", "[util_ringbuffer.hpp]" ) {
    butterfly::ringbuffer<std::string> buf( 10 );

    // Check that our buffer has a maximum of 10 elements with 0 already inserted
    REQUIRE( buf.max() == 10 );
    REQUIRE( buf.size() == 0 );

    // Insert 3
    buf.insert( "test1" );
    buf.insert( "test2" );
    buf.insert( "test3" );

    // Check that the size and content matches
    REQUIRE( buf.size() == 3 );

    REQUIRE( buf.at( 0 ) == "test1" );
    REQUIRE( buf.at( 1 ) == "test2" );
    REQUIRE( buf.at( 2 ) == "test3" );

    // this should replace all elements with test
    for ( uint32_t i = 0; i < 10; ++i ) {
        buf.insert( "test" );
    }

    // test that it really happened
    REQUIRE( buf.at( 0 ) == "test" );
    REQUIRE( buf.at( 9 ) == "test" );
    REQUIRE( buf.size() == 10 );

    // test that we can overwrite the last element
    buf.insert( "new" );

    REQUIRE( buf.at( 9 ) == "new" );

    // resize it to 15 elements
    buf.resize( 15 );

    // test that the old size is still true and the element order still matches
    REQUIRE( buf.size() == 10 );
    REQUIRE( buf.at( 0 ) == "test" );
    REQUIRE( buf.at( 9 ) == "new" );

    // add 5 new entries, these should not overwrite old ones
    for ( uint32_t i = 0; i < 5; ++i ) {
        buf.insert( "old" );
    }

    REQUIRE( buf.at( 0 ) == "test" );
    REQUIRE( buf.at( 9 ) == "new" );
    REQUIRE( buf.at( 14 ) == "old" );

    // overwrite the buffer multiple times with 'old2'
    for ( uint32_t i = 0; i < 50; ++i ) {
        buf.insert( "old2" );
    }

    // test that it worked
    REQUIRE( buf.at( 0 ) == "old2" );
    REQUIRE( buf.at( 14 ) == "old2" );
}
