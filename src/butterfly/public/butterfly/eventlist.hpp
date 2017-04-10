/**
 * @file eventlist.hpp
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

#ifndef BUTTERFLY_EVENTLIST_HPP
#define BUTTERFLY_EVENTLIST_HPP

#include <string>
#include <cstdint>

#include <butterfly/proto/gameevents.pb.h>
#include <butterfly/util_dict.hpp>

namespace butterfly {
    /** Eventlist, keeps track of possible events and their descriptors */
    class eventlist {
    public:
        /** Single event */
        struct descriptor {
            struct key {
                /** Key type */
                uint32_t type;
                /** Key name */
                std::string name;
            };

            /** Event name */
            std::string name;

            /** Event id */
            uint32_t id;

            /** List of event keys */
            std::vector<key> keys;
        };

        /** Load event list from buffer */
        void load_from_buffer( uint32_t size, char* buffer ) {
            events.clear();

            CMsgSource1LegacyGameEventList proto;
            proto.ParseFromArray( buffer, size );

            for ( auto& d : proto.descriptors() ) {
                descriptor desc;
                desc.name = d.name();
                desc.id   = d.eventid();

                for ( auto& k : d.keys() ) {
                    desc.keys.push_back( {static_cast<uint32_t>( k.type() ), k.name()} );
                }

                events.insert( d.eventid(), d.name(), std::move( desc ) );
            }
        }

        /** Returns event by it's name */
        descriptor* by_name( const std::string& name ) {
            ASSERT_TRUE( events.has_key( name ), "Unkown event key" );
            return &( events.by_key( name ).value );
        }

        /** Returns event by it's id */
        descriptor* by_id( uint32_t id ) {
            ASSERT_TRUE( events.has_index( id ), "Unkown event index" );
            return &( events.by_index( id ).value );
        }

    private:
        /** List of events */
        dict<descriptor> events;
    };
}

#endif /* BUTTERFLY_EVENTLIST_HPP */
