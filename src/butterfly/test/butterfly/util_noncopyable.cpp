/**
 * @file util_noncopyable.cpp
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

#include <type_traits>

#include <catch.hpp>
#include <butterfly/util_noncopyable.hpp>

using namespace butterfly;

TEST_CASE( "copying", "[util_noncopyable.hpp]" ) {
    class noncopyable_test : public noncopyable {};

    REQUIRE( std::is_copy_constructible<noncopyable_test>::value == false );
    // Not supported by Clang 3.7 REQUIRE( std::is_trivially_copy_constructible<noncopyable_test>::value == false );
    REQUIRE( std::is_copy_assignable<noncopyable_test>::value == false );
    // Not supported by Clang 3.7 REQUIRE( std::is_trivially_copy_assignable<noncopyable_test>::value == false );
}
