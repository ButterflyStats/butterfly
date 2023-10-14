/**
 * @file util_platform.cpp
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
#include <butterfly/util_platform.hpp>

// this has to compile
force_inline int add( int a, int b ) { return a + b; }

// expect keyword
TEST_CASE( "expect", "[util_platform.hpp]" ) {
    if ( expect( 1 != 1, false ) ) {
        REQUIRE_FALSE( true );
    }
}

// force_inline keyword
TEST_CASE( "force_inline", "[util_platform.hpp]" ) { REQUIRE( add( 1, 2 ) == 3 ); }
