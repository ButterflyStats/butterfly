/**
 * @file pyvisitor.hpp
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

#ifndef BUTTERFLY_PY_VISITOR_HPP
#define BUTTERFLY_PY_VISITOR_HPP

#include <butterfly/butterfly.hpp>
#include <cstdint>

namespace butterfly {
    /** Visitor implementation for python */
    class pyvisitor : public visitor {
    public:
        typedef std::function<void (uint32_t, char*, uint32_t)> cb_packet;
        typedef std::function<void (uint32_t)> cb_state;
        typedef std::function<void (entity_state state, entity* e)> cb_entity;
    public:
        virtual void on_packet( uint32_t id, char* data, uint32_t size ) final {
            if (!m_cb_packet) return;
            m_cb_packet(id, data, size);
        }

        /** Invoked on state change */
        virtual void on_state( uint32_t state ) final {
            if (!m_cb_state) return;
            m_cb_state(state);
        }

        /** Invoked on entity changes */
        virtual void on_entity( entity_state state, entity* ent ) final {
            if (!m_cb_entity) return;
            m_cb_entity(state, ent);
        }

        /** Set packet callback */
        void set_packet_cb(cb_packet cb) {
            m_cb_packet = cb;
        }

        /** Set state callback */
        void set_state_cb(cb_state cb) {
            m_cb_state = cb;
        }

        /** Set entity callback */
        void set_entity_cb(cb_entity cb) {
            m_cb_entity = cb;
        }
    private:
        cb_packet m_cb_packet;
        cb_state m_cb_state;
        cb_entity m_cb_entity;
    };
} /* butterfly */

#endif /* BUTTERFLY_PY_VISITOR_HPP */
