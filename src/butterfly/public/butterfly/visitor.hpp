/**
 * @file visitor.hpp
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

#ifndef BUTTERFLY_VISITOR_HPP
#define BUTTERFLY_VISITOR_HPP

#include <cstdint>

// forward decl
class CMsgSource1LegacyGameEvent;

namespace butterfly {
    // forward decl
    class entity;
    class parser;

    /** Possible entity states */
    enum entity_state { ENT_CREATED = 0, ENT_UPDATED = 1, ENT_DELETED = 2 };

    /** Visitor baseclass */
    class visitor {
    friend class parser;
    public:
        /** Invoked on incoming packages */
        virtual void on_packet( uint32_t id, char* data, uint32_t size ) {}

        /** Invoked on state change */
        virtual void on_state( uint32_t state ) {}

        /** Invoked on entity changes */
        virtual void on_entity( entity_state state, entity* ent ) {}

        /** Invoked on game events */
        virtual void on_event( CMsgSource1LegacyGameEvent* event ) {}

        /** Invoked when the tick changes */
        virtual void on_tick( int32_t tick ) {}

        /** Called everytime the parsing state advances */
        virtual void on_progress( float f ) {}

      protected:
          /** Pointer to parser */
          parser* p;
    };
} /* butterfly */

#endif /* BUTTERFLY_VISITOR_HPP */
