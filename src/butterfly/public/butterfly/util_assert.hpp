/**
 * @file util_assert.hpp
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

#ifndef BUTTERFLY_UTIL_ASSERT_HPP
#define BUTTERFLY_UTIL_ASSERT_HPP

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>

#include "util_platform.hpp"

/** Callback format on assertion */
typedef void ( *assert_cb )( const char* );

/** Global error handler */
extern assert_cb g_assertion_callback;

/** Assertion function */
static inline bool bf_assert( const char* format, ... ) {

    va_list args;
    va_start( args, format );

    if ( !g_assertion_callback ) {
        vprintf( format, args );
    } else {
        char buf[1024];
        vsnprintf( buf, 256, format, args );
        g_assertion_callback( buf );
    }

    va_end( args );
    exit( 1 );
    return false;
}

/// Terminates the program if X and Y are not equal
#define ASSERT_EQUAL( X, Y, MESSAGE )                                                                                  \
    do {                                                                                                               \
        expect( ( X ) == ( Y ), 1 ) ||                                                                                 \
            bf_assert( "%s:%d: Expected %s == %s: %s.\n", __FILE__, __LINE__, #X, #Y, MESSAGE );                       \
    } while ( 0 )

/// Terminates the program if X is not true
#define ASSERT_TRUE( X, MESSAGE )                                                                                      \
    do {                                                                                                               \
        expect( (bool)( X ), 1 ) || bf_assert( "%s:%d: %s.\n", __FILE__, __LINE__, MESSAGE );                          \
    } while ( 0 )

/// Terminates the program if X is not false
#define ASSERT_FALSE( X, MESSAGE )                                                                                     \
    do {                                                                                                               \
        expect( !( X ), 1 ) || bf_assert( "%s:%d: %s.\n", __FILE__, __LINE__, MESSAGE );                               \
    } while ( 0 )

/// Terminates the program if X is less than Y
#define ASSERT_GREATER( X, Y, MESSAGE )                                                                                \
    do {                                                                                                               \
        expect( ( X ) >= ( Y ), 1 ) ||                                                                                 \
            bf_assert( "%s:%d: Expected %s > %s: %s.\n", __FILE__, __LINE__, #X, #Y, MESSAGE );                        \
    } while ( 0 )

/// Terminates the program if X is greater than Y
#define ASSERT_LESS( X, Y, MESSAGE )                                                                                   \
    do {                                                                                                               \
        expect( ( X ) <= ( Y ), 1 ) ||                                                                                 \
            bf_assert( "%s:%d: Expected %s < %s: %s.\n", __FILE__, __LINE__, #X, #Y, MESSAGE );                        \
    } while ( 0 )

#endif /* BUTTERFLY_UTIL_ASSERT_HPP */
