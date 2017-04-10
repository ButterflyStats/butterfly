/**
 * @file util_delegate.cpp
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
#include <butterfly/util_delegate.hpp>

// counter we test against
uint32_t g_counter;

/** Void, no args */
void fcn_void_noargs() {
    g_counter += 1;
}

/** Void, args */
void fcn_void_args(int i) {
    g_counter += i;
}

/** Non-void, args */
int fcn_args(int i) {
    g_counter += i;
    return g_counter;
}

using namespace butterfly;

TEST_CASE( "delegate free functions", "[util_delegate.hpp]" ) {
    auto d1 = delegate<void()>::from<fcn_void_noargs>();
    auto d2 = delegate<void(int i)>::from<fcn_void_args>();
    auto d3 = delegate<int(int i)>::from<fcn_args>();

    g_counter = 0;
    d1();    REQUIRE( g_counter == 1);
    d2(10);  REQUIRE( g_counter == 11 );
    d3(10);  REQUIRE( g_counter == 21 );

    REQUIRE( g_counter == d3(0) );
}
