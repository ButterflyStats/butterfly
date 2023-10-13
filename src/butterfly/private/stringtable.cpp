/**
 * @file stringtable.cpp
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

/// Defines the maximum number of names to keep a history of
#define STRINGTABLE_NAME_HISTORY 32

#define STRINGTABLE_NAME_HISTORY_MASK (STRINGTABLE_NAME_HISTORY - 1)

/// Maximum length of a stringtable entry name
#define STRINGTABLE_MAX_NAME_SIZE 0x400

/// Maximum size of a stringtable entry value
#define STRINGTABLE_MAX_VALUE_SIZE 0x4000

#include <array>
#include <string>
#include <cmath>
#include <snappy.h>

#include <butterfly/util_bitstream.hpp>
#include <butterfly/util_assert.hpp>
#include <butterfly/stringtable.hpp>

namespace butterfly {
    stringtable::stringtable( CSVCMsg_CreateStringTable* table )
        : tblName( table->name() ), userDataSizeBits( table->user_data_size_bits() ),
          flags( table->flags() ), userDataFixed( table->user_data_fixed_size() ),
          usingVarintBitcounts( table->using_varint_bitcounts() ) {
        // Snappy within snappy because why not
        if ( table->data_compressed() ) {
            size_t size = 0;
            std::string data;
            const std::string& tbl = table->string_data();

            // verify and get length
            ASSERT_TRUE(
                snappy::IsValidCompressedBuffer( tbl.c_str(), tbl.size() ), "Invalid snappy compression buffer" );
            ASSERT_TRUE(
                snappy::GetUncompressedLength( tbl.c_str(), tbl.size(), &size ), "Unable to get uncompressed length" );

            // uncompress
            data.resize( size );
            ASSERT_TRUE( snappy::RawUncompress( tbl.c_str(), tbl.size(), &data[0] ), "Failed to decompress data" );
            update( table->num_entries(), data );
        } else {
            update( table->num_entries(), table->string_data() );
        }
    }

    void stringtable::update( CSVCMsg_UpdateStringTable* table ) {
        update( table->num_changed_entries(), table->string_data() );
    }

    void stringtable::update( const CDemoStringTables_table_t& tbl ) {
        table.clear();

        for ( auto& item : tbl.items() )
            table.emplace_back( item.str(), item.data() );
    }

    void stringtable::update( const uint32_t& entries, const std::string& data ) {
        // create bitstream for data field
        bitstream bstream( data );

        // index for consecutive incrementing
        int32_t index = -1;

        // value storage
        char value[STRINGTABLE_MAX_VALUE_SIZE] = {'\0'};

        // keeps track of the last names
        std::array<std::string, STRINGTABLE_NAME_HISTORY> names;
        uint32_t delta_pos = 0;

        // read all the entries in the string table
        for ( uint32_t i = 0; i < entries; ++i ) {
            if ( !bstream.readBool() )
                index += bstream.readVarUInt32() + 2;
            else
                index++;

            ASSERT_GREATER_EQ( index, 0, "Invalid stringtable index" );

            // reset value before re-reading it
            value[0] = '\0';

            // read name
            const bool hasName = bstream.readBool();
            auto& name = names[delta_pos & STRINGTABLE_NAME_HISTORY_MASK];
            if ( hasName ) {
                // check if the name begins with a stored substring
                const bool isSubstring    = bstream.readBool();
                const uint32_t delta_zero = delta_pos > STRINGTABLE_NAME_HISTORY ? delta_pos & STRINGTABLE_NAME_HISTORY_MASK : 0;
                
                name.clear(); // reset name before re-reading it

                if ( isSubstring ) {
                    const uint32_t sIndex  = ( delta_zero + bstream.read( 5 ) ) & STRINGTABLE_NAME_HISTORY_MASK;
                    const uint32_t sLength = bstream.read( 5 );

                    ASSERT_LESS( sIndex, STRINGTABLE_NAME_HISTORY, "Invalid stringtable name-delta specified" );

                    if ( delta_pos >= sIndex && names[sIndex].size() >= sLength )
                        name.assign( names[sIndex].c_str(), sLength );
                }
                bstream.readString( name, STRINGTABLE_MAX_NAME_SIZE );

                ++delta_pos;
            }

            // read value
            const bool hasValue = bstream.readBool();
            bool isCompressed   = false;
            uint32_t size       = 0;

            if ( hasValue ) {
                // memset( &value[0], 0, STRINGTABLE_MAX_VALUE_SIZE );

                if ( userDataFixed ) {
                    size = userDataSizeBits;
                    bstream.readBits( value, size );
                } else {
                    // check if table might be compressed
                    if (flags & 0x1) {
                        // this is the case for the instancebaseline for console recorded replays
                        isCompressed = bstream.readBool();
                    }

                    size = usingVarintBitcounts
                        ? bstream.readUBitVar()
                        : bstream.read( 17 );

                    ASSERT_TRUE( size < STRINGTABLE_MAX_VALUE_SIZE, "Decompressed stringtable to big (value)" );
                    bstream.readBytes( value, size );

                    if (isCompressed) {
                        size_t uncomp_size = 0;
                        std::string uncomp_data;

                        // verify and get length
                        ASSERT_TRUE( snappy::IsValidCompressedBuffer( value, size ), "Invalid snappy compression buffer (value)" );
                        ASSERT_TRUE( snappy::GetUncompressedLength( value, size, &uncomp_size ), "Unable to get uncompressed length (value)" );
                        ASSERT_TRUE( uncomp_size < STRINGTABLE_MAX_VALUE_SIZE, "Decompressed stringtable too big (value)" );

                        // uncompress
                        uncomp_data.resize( uncomp_size );
                        ASSERT_TRUE( snappy::RawUncompress( value, size, &uncomp_data[0] ), "Failed to decompress data (value)" );

                        // save to value
                        size = uncomp_size;
                        memcpy(value, uncomp_data.c_str(), size);
                    }
                }
            }

            // insert entry
            if ( index < table.size() ) {
                if ( hasName )
                    table[index].name = name;
                table[index].value.assign( value, size );
            } else {
                ASSERT_EQUAL( index, table.size(), "Stringtable inserts must be at the end" );
                table.emplace_back( name, std::string( value, size ) );
            }
        }
    }
} /* butterfly */
