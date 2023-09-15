/**
 * @file entity.hpp
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
 *     Entity related code
 */

#ifndef BUTTERFLY_ENTITY_HPP
#define BUTTERFLY_ENTITY_HPP

#include <string>
#include <memory>
#include <mutex>
#include <cstdint>
#include <vector>
#include <iostream>

#include <butterfly/util_chash.hpp>
#include <butterfly/entity_classes.hpp>
#include <butterfly/flattened_serializer.hpp>

/// Entity mask for ehandles
#define EMASK 0x3FFF
#define ENULL 16777215

namespace butterfly {
    // forward decl
    class bitstream;
    class resource_manifest;
    struct fs;

    /** Single networked entity */
    class entity {
    public:
        /** Baseline pointer, can be null */
        entity* baseline;
        /** Own entity ID in global list */
        uint32_t id;
        /** Class id */
        uint32_t cls : 24;
        /** Type */
        entity_types type : 8;
        /** Class hash */
        uint64_t cls_hash;
        /** Serializer */
        const fs* ser;

        /** Constructor */
        entity( const fs* ser );

        /** Destructor */
        ~entity();

        /** Copy constructor */
        entity( const entity& e );

        /** Parse entity data from bitstream */
        void parse( const resource_manifest& m, bitstream& b );

        /** Spew property to console */
        void spew( const flattened_serializer* serializers, std::ostream& out = std::cout );

        /** Get field by its path name hashes */
        template <typename T>
        std::pair<const fs*, const void*> getRaw( const T& path ) const {
            if ( path.empty() )
                return { ser, properties };

            auto table_data = ser->table_data;
            auto prop_idx   = table_data->properties_map.find( path[0] );
            ASSERT_TRUE( prop_idx != table_data->properties_map.end(),
                         "Trying to access invalid property" );
            auto prop_fs   = table_data->properties[prop_idx->second];
            auto prop_addr = &properties[prop_fs->offset];
            for ( size_t i = 1; i < path.size(); i++ ) {
                bool in_array = prop_fs->is_array();
                if ( in_array ) {
                    prop_fs = prop_fs->array_prop;
                    auto& ar =
                        *reinterpret_cast<field_array_container*>( prop_addr );
                    if ( ar.size <= path[i] )
                        return { nullptr, nullptr };
                    prop_addr = &ar.data[path[i] * prop_fs->size];
                } else {
                    ASSERT_TRUE( prop_fs->is_table(),
                                 "Trying to access property of a primitive type" );
                    table_data = prop_fs->table_data;
                    prop_idx   = table_data->properties_map.find( path[i] );
                    ASSERT_TRUE( prop_idx != table_data->properties_map.end(),
                                 "Trying to access invalid property" );
                    prop_fs   = table_data->properties[prop_idx->second];
                    prop_addr = &prop_addr[prop_fs->offset];
                }
            }

            return { prop_fs, prop_addr };
        }

        /** Get field by its name hash */
        std::pair<const fs*, const void*> getRaw( uint64_t hash ) const {
            return getRaw( std::array<uint64_t, 1>{ hash } );
        }
        
        /** Get field by its path name hashes */
        template <typename T, typename P>
        T get( const P& path ) const {
            auto prop      = getRaw<P>( path );
            auto prop_fs   = prop.first;
            auto prop_addr = prop.second;

            if ( prop_fs == nullptr )
                return T();

            ASSERT_FALSE( prop_fs->is_array(),
                          "Trying to resolve an array as a regular property" );
            ASSERT_FALSE( prop_fs->is_table(),
                          "Trying to resolve a table as a regular property" );

            if ( prop_addr == nullptr )
                return T();

            return prop_fs->info->template extract_value<T>( prop_addr );
        }

        /** Get field by its name hash */
        template <typename T>
        T get( uint64_t hash ) const {
            return get<T>( std::array<uint64_t, 1>{ hash } );
        }

        /** Get field by string */
        template <typename T>
        T get( const std::string& s ) const {
            return get<T>( constexpr_hash_rt( s.c_str() ) );
        }

        /** Returns true if field exists */
        bool has( uint64_t hash ) const { return getRaw( hash ).second != nullptr; }

        /** Returns true if field exists */
        bool has( const std::string& name ) const {
            return has( constexpr_hash_rt( name.c_str() ) );
        }

        /** Returns true if field exists */
        template <typename T>
        bool has( const T& path ) const {
            return getRaw( path ).second != nullptr;
        }

    private:
        void init_props( uint8_t* ptr, const fs* f );
        void deinit_props( uint8_t* ptr, const fs* f );
        void move_props( uint8_t* old_ptr, uint8_t* new_ptr, const fs* f );
        void copy_props( uint8_t* old_ptr, uint8_t* new_ptr, const fs* f );

        /** Properties */
        uint8_t* properties;
    };
} /* butterfly */

#endif /* BUTTERFLY_ENTITY_HPP */
