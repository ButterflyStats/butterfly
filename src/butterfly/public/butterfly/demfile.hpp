/**
 * @file demfile.hpp
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
 *    .dem format related code, such as a parser for the basic file structure.
 */

#ifndef BUTTERFLY_DEMFILE_HPP
#define BUTTERFLY_DEMFILE_HPP

#include <string>
#include <cstddef>

#include <butterfly/proto/demo.pb.h>
#include <butterfly/dem.hpp>
#include <butterfly/util_noncopyable.hpp>

namespace butterfly {
    /**
     * Provides the functionallity to verify and read from a single .dem file.
     * Can be used without the parser, see examples/02-spew-types.cpp on how to do that.
     */
    class demfile : private noncopyable {
    public:
        /** Default move constructor */
        demfile( demfile&& ) = default;

        /** Default move assignment operator */
        demfile& operator=( demfile&& ) = default;

        /** Loads specified file into memory to be parsed */
        demfile( const char* path, std::function<void (float)> = nullptr );

        /** Read from the provided buffer, if pcb is 0, no progress will be reported. */
        demfile( char* data, std::size_t size, std::function<void (float)> = nullptr );

        /** Destructor */
        ~demfile();

        /** Returns a single dem packet */
        dem_packet get();

        /** Whether there is still data left to read */
        bool good();

        /** Returns the current position */
        std::size_t pos();

        /** Set current position */
        void set_pos( std::size_t p );

        /** Return game summary */
        CDemoFileInfo summary();

    private:
        /** Data buffer */
        char* data;
        /** Data buffer size */
        std::size_t dataSize;
        /** Current position */
        std::size_t dataPos;
        /** Buffer for uncompressed snappy data */
        char* dataSnappy;
        /** Whether we own the underlying buffer */
        bool ownsBuffer;
        /** Summary offset */
        std::size_t offset;
        /** Progress callback */
        std::function<void (float)> pcb;

        /** Verifies the file signature */
        void parse_header();
    };
} /* butterfly */

#endif /* BUTTERFLY_DEMFILE_HPP */
