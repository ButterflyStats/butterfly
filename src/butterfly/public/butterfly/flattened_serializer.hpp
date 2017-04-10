/**
 * @file flattened_serializer.hpp
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
 *    Code used to handle Source 2's new flattened serializer structure and related class information.
 */

#ifndef BUTTERFLY_FLATTENED_SERIALIZER_HPP
#define BUTTERFLY_FLATTENED_SERIALIZER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>

#include <butterfly/proto/netmessages.pb.h>
#include <butterfly/util_assert.hpp>
#include <butterfly/util_dict.hpp>
#include <butterfly/util_noncopyable.hpp>
#include <butterfly/property_decoder.hpp>

namespace butterfly {
    // forward decl
    struct entity_classes;

    /** Field information */
    struct fs_info {
        /** Name */
        std::string name;
        /** Original field index */
        uint32_t field;
        /** Encoder hash */
        uint64_t encoder;
        /** Outer type hash */
        uint64_t type;
        /** Bits used to encode type */
        uint32_t bits : 31;
        /** Whether this property is dynamic */
        bool dynamic : 1;
        /** Encoding flags */
        uint32_t flags;
        /** Min val */
        float min;
        /** Max val */
        float max;
    };

    /** Type information about a property */
    struct fs_typeinfo {
        /** Whether this property is pointing at another class */
        bool is_table : 1;
        /** Index of said table */
        uint16_t table : 15;
        /** Whether this property is dynamic */
        bool is_dynamic : 1;
        /** Number of members if this is an array */
        uint16_t size : 15;
        /** Decoder */
        decoder_fcn* decoder;
        /** Normal information */
        fs_info* info;
    };

    /** Flattened serializer table for a single networked class, points to decoder and property */
    struct fs {
        /** Returns field at given position */
        const fs& operator[]( uint32_t n ) const {
            if ( info->dynamic && properties.size() == 1 ) {
                return properties[0];
            }

            ASSERT_TRUE( n < properties.size(), "FS out-of-bounds" );
            return properties[n];
        }

        /** List of fields / props */
        std::vector<fs> properties;
        /** Pointer to decoder */
        decoder_fcn* decoder;
        /** Pointer to field info */
        fs_info* info;
        /** FS name */
        std::string name;
        /** Hash */
        uint64_t hash;
    };

    /**  Flattened serializer structure introduced in Source 2 */
    class flattened_serializer : noncopyable {
    public:
        /** Initialize serializer from data */
        flattened_serializer( uint8_t* data, uint32_t size );

        /** Destructor */
        ~flattened_serializer();

        /** Build serializers for given entity classes */
        void build( entity_classes& cls );

        /** Dump serializer at given index to console */
        void spew( uint32_t idx, std::ostream& out = std::cout );

        /** Return serializer at given index */
        const fs& get( uint32_t idx );

        /** Returns original type-symbol as string */
        std::string get_otype( fs_info* f );

    private:
        /** Serializer data from replay */
        CSVCMsg_FlattenedSerializer serializers;
        /** Flattening tables */
        std::vector<fs> tables;
        /** Tables indexed by name and position */
        dict<fs> tables_internal;
        /** Stores metadata per property */
        std::unordered_map<uint32_t, fs_typeinfo> metadata;

        /** Spew implementation */
        void spew_impl( uint32_t idx, void* tbl, std::string target = "", bool internal = false );
        /** Returns metadata for field */
        fs_typeinfo& get_metadata( uint32_t field );
        /** Fill string information */
        void app_name_hash( std::string n, fs& f );
    };
} /* butterfly */

#endif /* BUTTERFLY_FLATTENED_SERIALIZER_HPP */
