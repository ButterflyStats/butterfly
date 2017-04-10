/**
 * @file dem.hpp
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
 *
 * @par Description
 *    Contains definitions for the demo file header and the individual packets, as well as functions to uncompress
 *    and read nested buffers.
 */

#ifndef BUTTERFLY_DEM_HPP
#define BUTTERFLY_DEM_HPP

#include <cstdint>
#include <cstdio>

namespace butterfly {
    /** DEM file header, used for verification purposes */
    struct dem_header {
        /** Used for verification purposes, needs to equal DOTA_HEADERID */
        char headerid[8];
        /** Points to the location of the game summary */
        int32_t offset_summary;
        /** Points to another packet @todo: verify */
        int32_t offset_2;
    };

    /** A single dem message */
    struct dem_packet {
        /** Tick this messages was emitted at */
        int32_t tick;
        /** Message type */
        uint32_t type;
        /** Size of data */
        uint32_t size;
        /** Points to start of message */
        char* data;
    };

    /** Decompresses given message */
    void dem_uncompress( dem_packet& msg, char* buffer, size_t buffer_size );

    /** Reads data from buffer into msg and returns bytes read */
    size_t dem_from_buffer( dem_packet& msg, char* buffer, size_t buffer_size, bool read_tick = false );

    /** Progress callback, float value is in the range of 0.0f to ~100.0f */
    typedef void ( *dem_progress_cb )( float );
} /* butterfly */

#endif /* BUTTERFLY_DEM_HPP */
