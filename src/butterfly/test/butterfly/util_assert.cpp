/**
 * @file util_assert.cpp
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
#include <butterfly/util_assert.hpp>

// This is somewhat of an unusual test as ASSERT_X calls std::terminate internally, so no further tests will be
// executed if this one fails.

// Test Equal
TEST_CASE( "assert_equal", "[util_assert.hpp]" ) { ASSERT_EQUAL( 1, 1, "" ); }

// Test True
TEST_CASE( "assert_true", "[util_assert.hpp]" ) { ASSERT_TRUE( 1, "" ); }

// Test False
TEST_CASE( "assert_false", "[util_assert.hpp]" ) { ASSERT_FALSE( 0, "" ); }

// Test Less
TEST_CASE( "assert_less", "[util_assert.hpp]" ) { ASSERT_LESS( 0, 1, "" ); }

// Test LessEq
TEST_CASE( "assert_less_eq", "[util_assert.hpp]" ) {
    ASSERT_LESS_EQ( 0, 0, "" );
    ASSERT_LESS_EQ( 0, 1, "" );
}

// Test Greater
TEST_CASE( "assert_greater", "[util_assert.hpp]" ) { ASSERT_GREATER( 1, 0, "" ); }

// Test GreaterEq
TEST_CASE( "assert_greater_eq", "[util_assert.hpp]" ) {
    ASSERT_GREATER_EQ( 0, 0, "" );
    ASSERT_GREATER_EQ( 1, 0, "" );
}
