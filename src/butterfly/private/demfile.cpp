/**
 * @file demfile.cpp
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

#include <functional>
#include <string>
#include <cstdint>
#include <cstdio>

#include <butterfly/proto/demo.pb.h>
#include <butterfly/dem.hpp>
#include <butterfly/demfile.hpp>
#include <butterfly/util_assert.hpp>

#include "config_internal.hpp"

namespace butterfly {
    demfile::demfile( const char* path, std::function<void (float)> pcb )
        : data( nullptr ), dataSize( 0 ), dataPos( 0 ), dataSnappy( new char[BUTTERFLY_SNAPPY_BUFFER_SIZE] ),
          ownsBuffer( true ), offset( 0 ), pcb( pcb ) {
        // Use C-Style reading instead of ifstreams for performance
        FILE* fp = fopen( path, "rb" );

        ASSERT_TRUE( fp, "Error opening file" );

        const auto fstart = ftell( fp );
        fseek( fp, 0, SEEK_END );
        dataSize = ftell( fp ) - fstart;
        fseek( fp, 0, SEEK_SET );

        ASSERT_GREATER_EQ( dataSize, sizeof( dem_header ), "File to small" );

        // read everything into the buffer
        data = new char[dataSize + 1];

        // mutable vars
        uint32_t dataSizeM = dataSize;
        char* dataM        = data;

        // read in chunks for a slight performance improvement
        while ( dataSizeM > BUTTERFLY_IFSTREAM_CHUNK_SIZE ) {
            ASSERT_TRUE( fread( dataM, 1, BUTTERFLY_IFSTREAM_CHUNK_SIZE, fp ), "Unable to read from demo file" );
            dataM = dataM + BUTTERFLY_IFSTREAM_CHUNK_SIZE;
            dataSizeM -= BUTTERFLY_IFSTREAM_CHUNK_SIZE;
        }

        ASSERT_TRUE( fread( dataM, 1, dataSizeM, fp ), "Unable to read from demo file" );
        fclose( fp );

        // verify header
        parse_header();
    }

    demfile::demfile( char* data, std::size_t size, std::function<void (float)> pcb )
        : data( data ), dataSize( size ), dataPos( 0 ), dataSnappy( new char[BUTTERFLY_SNAPPY_BUFFER_SIZE] ),
          ownsBuffer( false ), pcb( pcb ) {
        // verify header
        parse_header();
    }

    demfile::~demfile() {
        if ( ownsBuffer && data )
            delete[] data;

        if ( dataSnappy )
            delete[] dataSnappy;
    }

    dem_packet demfile::get() {
        ASSERT_TRUE( dataPos <= dataSize, "Trying to read from invalid buffer" );

        dem_packet ret;
        dataPos += dem_from_buffer( ret, data + dataPos, dataSize - dataPos, true );

        if ( ret.type & DEM_IsCompressed ) {
            dem_uncompress( ret, dataSnappy, BUTTERFLY_SNAPPY_BUFFER_SIZE );

            ASSERT_LESS( ret.type, DEM_Max, "Unknown demo packet received" );
        }

        if ( pcb ) {
            float prog = ( (float)dataPos / (float)dataSize ) * 100.0f;
            pcb( prog );
        }

        return ret;
    }

    bool demfile::good() { return ( dataPos < dataSize ); }

    std::size_t demfile::pos() { return dataPos; }

    void demfile::set_pos( std::size_t p ) {
        ASSERT_TRUE( p < dataSize, "Overflow when setting demfile position" );
        dataPos = p;
    }

    CDemoFileInfo demfile::summary() {
        ASSERT_TRUE( offset != 0, "No summary offset found in header" );

        auto oPos = dataPos;
        dataPos   = offset;

        auto pkg = get();
        dataPos  = oPos;

        if ( pkg.type & DEM_IsCompressed ) {
            dem_uncompress( pkg, dataSnappy, BUTTERFLY_SNAPPY_BUFFER_SIZE );
            ASSERT_LESS( pkg.type, DEM_Max, "Unknown demo packet received" );
        }

        ASSERT_TRUE( pkg.type == DEM_FileInfo, "Unkown packet at summary offset" );

        CDemoFileInfo proto;
        proto.ParseFromArray( pkg.data, pkg.size );

        return proto;
    }

    void demfile::parse_header() {
        // load header
        dem_header* head = (dem_header*)data;

        ASSERT_EQUAL( std::string( head->headerid ), std::string( BUTTERFLY_S2_HEADER ), "Invalid header ID" );
        offset = head->offset_summary;

        // increase position
        dataPos += sizeof( dem_header );
    }
} /* butterfly */
