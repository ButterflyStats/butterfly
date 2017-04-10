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
#include <mutex>
#include <unordered_map>
#include <iostream>

#include <butterfly/util_chash.hpp>
#include <butterfly/property.hpp>

/// Entity mask for ehandles
#define EMASK 0x3FFF
#define ENULL 16777215

namespace butterfly {
    // forward decl
    class bitstream;
    struct fs;

    /** Single networked entity */
    class entity {
    public:
        /** Properties, indexes by their hash */
        std::unordered_map<uint64_t, property*> properties;
        /** Baseline pointer, can be null */
        entity* baseline;
        /** Own entity ID in global list */
        uint32_t id;
        /** Class id */
        uint32_t cls : 24;
        /** Type */
        uint32_t type : 8;
        /** Class hash */
        uint64_t cls_hash;

        /** Constructor */
        entity();

        /** Destructor */
        ~entity();

        /** Copy constructor */
        entity(const entity& e);

        /** Set serialzier */
        void set_serializer( const fs* serializer );

        /** Parse entity data from bitstream */
        void parse( bitstream& b );

        /** Spew property to console */
        void spew(std::ostream& out = std::cout);

        /** Returns true if field exists */
        bool has( uint64_t i ) {
            auto i1 = properties.find( i );
            if ( i1 != properties.end() ) {
                return true;
            }

            return false;
        }

        /** Returns true if field exists */
        bool has( const std::string& s ) { return has( constexpr_hash_rt( s.c_str() ) ); }

        /** Get field by id */
        property* get( uint64_t i ) {
            auto i1 = properties.find( i );
            if ( i1 != properties.end() ) {
                return i1->second;
            }

            ASSERT_TRUE( 0 != 0, "Trying to access invalid property" );
            return nullptr;
        }

        /** Get field by string */
        property* get( const std::string& s ) { return get( constexpr_hash_rt( s.c_str() ) ); }

    private:
        /** Serializer */
        const fs* ser;
        /** Mutex */
        std::mutex mut;
    };
} /* butterfly */

#endif /* BUTTERFLY_ENTITY_HPP */
