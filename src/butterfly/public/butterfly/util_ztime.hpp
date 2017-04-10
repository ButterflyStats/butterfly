/**
 * @file util_ztime.hpp
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

#ifndef BUTTERFLY_UTIL_ZTIME_HPP
#define BUTTERFLY_UTIL_ZTIME_HPP

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <cstdio>
#include <cstdint>

#define BUTTERFLY_BENCHMARK 1
#define TIME_USEC_PER_SEC 1000000

// Benchmark start and stop macros
#if BUTTERFLY_BENCHMARK

#define BENCHMARK_START( name_ ) butterfly::ztime_t name_ = butterfly::getZTime();
#define BENCHMARK_END( name_ )                                                                                         \
    do {                                                                                                               \
        butterfly::ztime_t diff = butterfly::getZTime() - name_;                                                       \
        printf( "Benchmark [%s]: %fs\n", #name_, diff / 1000000.0f );                                                  \
    } while ( 0 );

#else /* BUTTERFLY_BENCHMARK */

#define BENCHMARK_START( name )
#define BENCHMARK_END( name )

#endif /* BUTTERFLY_BENCHMARK */

namespace butterfly {
    /** Type representing the format for a usec */
    typedef uint64_t ztime_t;

    /**
     * Returns high resolution time for performance counters.
     *
     * The actual values returned by this function may differ depending
     * on the operating system.
     */
    inline ztime_t getZTime() {
#ifdef WIN32
        ztime_t ztimep;
        QueryPerformanceCounter( (LARGE_INTEGER*)&ztimep );
        return ztimep;
#else
        struct timeval tv;
        gettimeofday( &tv, NULL );
        return ( (ztime_t)tv.tv_sec * TIME_USEC_PER_SEC ) + tv.tv_usec;
#endif
    }
} /* butterfly */

#endif /* BUTTERFLY_UTIL_ZTIME_HPP */
