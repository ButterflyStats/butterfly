/**
 * @file util_string.cpp
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
#include "../../private/util_string.hpp"

using namespace butterfly;

TEST_CASE( "trim", "[util_string.hpp]" ) {
    std::string lspace = " left";
    std::string rspace = "right ";
    std::string both   = " both ";

    REQUIRE( ltrim( lspace ) == "left" );
    REQUIRE( rtrim( rspace ) == "right" );
    REQUIRE( trim( both ) == "both" );
}

TEST_CASE( "replace_all", "[util_string.hpp]" ) {
    std::string rall = "r1 r2 r3";

    replace_all( rall, "r", "s" );
    REQUIRE( rall.compare( "s1 s2 s3" ) == 0 );
}
