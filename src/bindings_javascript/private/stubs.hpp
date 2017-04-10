/**
 * @file stubs.hpp
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
 *    Stubs for butterfly classes.
 */

#include <vector>
#include <string>
#include <cstdint>
#include <sstream>

#include <emscripten.h>
#include <emscripten/bind.h>

#include <butterfly/entity.hpp>
#include <butterfly/property.hpp>

namespace butterfly {
    /** Wrapper around property */
    class jsProperty {
    public:
        jsProperty(property* p) : m_p(p) {}

        auto type() -> decltype(property::type) {
            return m_p->type;
        }

        auto data() -> decltype(property::data) {
            return m_p->data;
        }

        auto data_str() -> decltype(property::data_str) {
            return m_p->data_str;
        }

        auto info() -> decltype(property::info) {
            return m_p->info;
        }
    private:
        property* m_p;
    };

    /** Wrapper around entity */
    class jsEntity {
    public:
        struct jsPropHash {
            uint64_t hash;
        };

        jsEntity(entity* e) : m_e(e) {}

        bool valid() {
            return (m_e != nullptr);
        }

        auto id() -> decltype(entity::id) {
            return m_e->id;
        }

        auto cls_hash() -> decltype(entity::cls_hash) {
            return m_e->cls_hash;
        }

        auto cls() -> decltype(entity::cls) {
            return m_e->cls;
        }

        entity_types type() {
            return (entity_types)m_e->type;
        }

        void spew() {
            m_e->spew();
        }

        std::string spew_string() {
            std::stringstream s("");
            m_e->spew(s);
            return s.str();
        }

        bool has(std::string s) {
            return m_e->has(s);
        }

        bool has_hash(jsPropHash& h) {
            return m_e->has(h.hash);
        }

        jsProperty get(std::string s) {
            return jsProperty(m_e->get(s));
        }

        jsProperty get_by_hash(jsPropHash h) {
            return jsProperty(m_e->get(h.hash));
        }

        std::vector<jsPropHash> properties() {
            std::vector<jsPropHash> ret;
            ret.reserve(m_e->properties.size());

            for (auto &e : m_e->properties) {
                ret.push_back({e.first});
            }

            return ret;
        }
    private:
        entity* m_e;
    };

    /** Wrapper around visitor */
    struct jsVisitor : public wrapper<visitor> {
        EMSCRIPTEN_WRAPPER(jsVisitor);

        void on_packet( uint32_t id, char* data, uint32_t size ) {
            auto proto = parse_proto(id, data, size);

            if (proto) {
                return call<void>("on_packet", id, pb2js(proto));
            } else {
                return call<void>("on_packet", id, val::null());
            }
        }

        void on_state( uint32_t state ) {
            return call<void>("on_state", state);
        }

        void on_entity( entity_state state, entity* ent ) {
            return call<void>("on_entity", state, jsEntity(ent));
        }

        void on_tick( int32_t tick ) {
            return call<void>("on_tick", tick);
        }

        void on_progress( float perc ) {
            return call<void>("on_progress", perc);
        }
    };
}
