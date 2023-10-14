/**
 * @file util_platform.hpp
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

#ifndef BUTTERFLY_UTIL_PLATFORM_HPP
#define BUTTERFLY_UTIL_PLATFORM_HPP

#ifdef __GNUC__
#define expect( __expr, __c ) __builtin_expect( ( __expr ), ( __c ) )
#define force_inline __attribute__( ( always_inline ) ) inline
#else
#define expect( __expr, __c ) __expr
#define force_inline inline
#include <intrin.h>
#endif /* __GNUC */

force_inline uint32_t required_bits( uint32_t x ) {
    if ( x <= 1 )
        return 1;
#ifndef __GNUC__
    unsigned long result;
    _BitScanReverse( &result, x - 1 );
    return 1 + static_cast<uint32_t>( result );
#else
    return 32 - __builtin_clz( x - 1 );
#endif /* __GNUC */
}

#endif /* BUTTERFLY_UTIL_PLATFORM_HPP */
