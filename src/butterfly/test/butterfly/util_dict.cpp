/**
 * @file util_dict.cpp
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
#include <butterfly/util_dict.hpp>

using namespace butterfly;

struct dict_test {
    int ival;
};

TEST_CASE( "dict_standard", "[util_dict.hpp]" ) {
    typedef uint32_t value_t;

    dict<value_t> MyDict;
    MyDict.insert( 0, "V1", (uint32_t)3 );
    MyDict.insert( 1, "V2", (uint32_t)1 );
    MyDict.insert( 10, "V3", (uint32_t)2 );

    value_t v1 = MyDict.by_index( 0 ).value;

    REQUIRE( MyDict.by_index( 0 ).value == 3 );
    REQUIRE( MyDict.by_index( 0 ).key == "V1" );
    REQUIRE( MyDict.by_index( 1 ).value == 1 );
    REQUIRE( MyDict.has_key( "V3" ) );
    REQUIRE( MyDict.by_key( "V3" ).value == 2 );
    REQUIRE( MyDict.size() == 11 ); // [10] is V3, 2-9 is filled with garbage
}

TEST_CASE( "dict_dereference", "[util_dict.hpp]" ) {
    dict_test t1{1};
    dict_test t2{2};

    dict<dict_test> d1;
    d1.insert( 0, "0", t1 );

    dict<dict_test*> d2;
    d2.insert( 0, "0", &t2 );

    REQUIRE( d1.by_index( 0 ).value.ival == 1 );
    REQUIRE( d2.by_index( 0 ).value->ival == 2 );
    REQUIRE( d1.by_index( 0 )->ival == 1 );
    REQUIRE( d2.by_index( 0 )->ival == 2 );
}
