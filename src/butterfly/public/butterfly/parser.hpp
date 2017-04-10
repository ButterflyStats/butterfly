/**
 * @file parser.hpp
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

#ifndef BUTTERFLY_PARSER_HPP
#define BUTTERFLY_PARSER_HPP

#include <vector>
#include <cstdint>

#include <butterfly/dem.hpp>
#include <butterfly/demfile.hpp>
#include <butterfly/entity.hpp>
#include <butterfly/entity_classes.hpp>
#include <butterfly/eventlist.hpp>
#include <butterfly/packets.hpp>
#include <butterfly/particle.hpp>
#include <butterfly/stringtable.hpp>
#include <butterfly/util_dict.hpp>
#include <butterfly/util_noncopyable.hpp>

namespace butterfly {
    /// Forward decl
    class flattened_serializer;
    class visitor;
    struct fs;

    /** Entry point for the replay parser */
    class parser : private noncopyable {
    public:
        /** Demofile pointer */
        demfile* dem;
        /** Server-build the replay was generated with */
        uint32_t buildnumber;
        /** List of stringtables */
        dict<stringtable> stringtables;
        /** Entity classes */
        entity_classes classes;
        /** Serializers */
        flattened_serializer* serializers;
        /** Particle manager */
        particle_manager particles;
        /** List of baselines */
        std::vector<entity*> baselines;
        /** List of entities */
        std::vector<entity*> entities;
        /** Event list */
        eventlist events;
        /** Current tick */
        int32_t tick;

        /** Parser states */
        enum state {
            BEGIN      = 1, // Fired at the begining of a new replay
            SENDTABLES = 2, // Fired once sendtables are available
            END        = 3  // Fire when replay is finished
        };

        /** Information about min / max seeking */
        struct seekinfo {
            float gamestart;
            float pregamestart;
        };

        /** Constructor */
        parser();

        /** Destructor */
        virtual ~parser();

        /** Open demo file */
        void open( const char* path, visitor* v = nullptr );

        /** Reset parser state */
        void reset();

        /** Parse a single packet */
        void parse( visitor* v );

        /** Parses everything */
        void parse_all( visitor* v );

        /** Enabled forwarding of given packet id */
        void require( uint32_t id );

        /** Seek to the given second in the replay */
        void seek( uint32_t time );

    private:
        /** Intial seek position*/
        uint32_t seekPos;

        /** Packets that are being forwarded */
        std::vector<bool> packets;

        /**
         * Handles the demo file header.
         *
         * The packet is useful for determining the server build number, which allows us to
         * provide some backwards compatibility for old replays.
         */
        void dem_handle_file_header( dem_packet& p );

        /** Handles the sendtable packet */
        void dem_handle_send_tables( dem_packet& p );

        /** Handles the class info, maps all networked classes to their numeric ID */
        void dem_handle_class_info( dem_packet& p );

        /** Handles all packets */
        void dem_handle_packet( bitstream& bs, visitor* v );

        /** Handles stringtable creation */
        void svc_handle_stringtable_create( const char* data, uint32_t size );

        /** Handles stringtable updates */
        void svc_handle_stringtable_update( const char* data, uint32_t size );

        /** Handles entitie updates */
        void svc_handle_entities( const char* data, uint32_t size, visitor* v );
    };
} /* butterfly */

#endif /* BUTTERFLY_PARSER_HPP */
