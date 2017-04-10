/**
 * @file api0.cpp
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
 *    Javascript bindings for butterfly. Developer Edition (API Level 0).
 */

// Enable developer exports
#define DEVELOPER 1

#include <emscripten.h>
#include <emscripten/bind.h>

#include <butterfly/butterfly.hpp>

#include "protobuf.hpp"
#include "stubs.hpp"
#include "util.hpp"

using namespace butterfly;
using namespace emscripten;

EMSCRIPTEN_BINDINGS(butterfly) {
    /// ----------------------------------------------------------------
    /// Mount the filesystem (nodeFS and tempFS)
    /// ----------------------------------------------------------------

    EM_ASM(
        FS.mkdir("/node_root");
        FS.mkdir("/node_cwd");
        FS.mkdir("/web_root");

        if (ENVIRONMENT_IS_NODE) {
            FS.mount(NODEFS, { root: '/' }, '/node_root');
            FS.mount(NODEFS, { root: '.' }, '/node_cwd');
        }

        FS.mount(MEMFS, { root: '/' }, '/web_root');
    );

    /// ----------------------------------------------------------------
    /// Containers
    /// ----------------------------------------------------------------

    bind_dict<std::string>("stringDict");
    bind_dict<stringtable>("stringtableDict");
    bind_dict<entity_classes::class_info>("classInfoDict");

    register_vector<jsEntity::jsPropHash>("propVec");
    register_vector<std::size_t>("sizeTVector");
    register_vector<entity*>("entityVector");
    register_vector<fs>("fsVector");

    /// ----------------------------------------------------------------
    /// dem.hpp
    /// ----------------------------------------------------------------

    value_object<dem_header>("dem_header")
        // @ TODO .field("headerid", &dem_header::headerid)
        .field("offset_summary", &dem_header::offset_summary)
        .field("offset_2", &dem_header::offset_2);

    class_<dem_packet>("dem_packet")
        .property("tick", &dem_packet::tick)
        .property("type", &dem_packet::type)
        .property("size", &dem_packet::size)
        .function("data", select_overload<void* (const dem_packet&)>(
            [] (const dem_packet& self)
            {
                return (void*)self.data;
            }), allow_raw_pointer<arg<0>>()
        );

    emscripten::function("dem_uncompress", &dem_uncompress, allow_raw_pointer<arg<1>>());
    emscripten::function("dem_from_buffer", &dem_uncompress, allow_raw_pointer<arg<1>>());

    /// ----------------------------------------------------------------
    /// demfile.hpp
    /// ----------------------------------------------------------------

    class_<demfile>("demfile")
        // constructor (buffer, size, progress)
        .constructor<char*, std::size_t, dem_progress_cb>()
        .constructor<char*, std::size_t>(select_overload<demfile* (char*, std::size_t)>(
            [](char* data, std::size_t size) {
                return new demfile(data, size, nullptr);
            }), allow_raw_pointer<arg<0>>()
        )

        // body
        .function("get", &demfile::get)
        .function("good", &demfile::good)
        .function("pos", &demfile::pos)
        .function("set_pos", &demfile::set_pos);

    /// ----------------------------------------------------------------
    /// entity_classes.hpp
    /// ----------------------------------------------------------------

    class_<entity_classes>("entity_classes")
        .property("data", &entity_classes::classes)
        .function("is_networked", &entity_classes::is_networked)
        .function("class_id", &entity_classes::class_id)
        .function("bits", &entity_classes::bits);

    class_<entity_classes::class_info>("class_info")
        .property("hash", &entity_classes::class_info::hash)
        .property("type", &entity_classes::class_info::type);

    /// ----------------------------------------------------------------
    /// entity.hpp
    /// ----------------------------------------------------------------

    enum_<entity_types>("entity_types")
        .value("ENT_DEFAULT", ENT_DEFAULT)
        .value("ENT_ABILITY", ENT_ABILITY)
        .value("ENT_NPC", ENT_NPC)
        .value("ENT_HERO", ENT_HERO)
        .value("ENT_ITEM", ENT_ITEM)
        .value("ENT_UNIT", ENT_UNIT);

    class_<jsEntity::jsPropHash>("propHash");

    class_<jsEntity>("entity")
        .function("valid", &jsEntity::valid)
        .function("id", &jsEntity::id)
        .function("cls_hash", &jsEntity::cls_hash)
        .function("cls", &jsEntity::cls)
        .function("type", &jsEntity::type)
        .function("spew", &jsEntity::spew)
        .function("spew_string", &jsEntity::spew_string)
        .function("has", &jsEntity::has)
        .function("has_hash", &jsEntity::has_hash)
        .function("get", &jsEntity::get)
        .function("get_by_hash", &jsEntity::get_by_hash)
        .function("properties", &jsEntity::properties);

    /// ----------------------------------------------------------------
    /// flattened_serializer.hpp
    /// ----------------------------------------------------------------

    class_<fs_info>("fs_info")
        .property("name", &fs_info::name)
        .property("field", &fs_info::field)
        .property("encoder", &fs_info::encoder)
        .property("type", &fs_info::type)
        .property("flags", &fs_info::flags)
        .property("min", &fs_info::min)
        .property("max", &fs_info::max)
        .function("bits", select_overload<uint32_t (fs_info&)>(
            [](fs_info& f){ return f.bits; }
        ))
        .function("dynamic", select_overload<bool (fs_info&)>(
            [](fs_info& f){ return f.dynamic; }
        ));

    class_<fs_typeinfo>("fs_typeinfo")
        .function("is_table", select_overload<bool (fs_typeinfo&)>(
            [](fs_typeinfo& f){ return f.is_table; }
        ))
        .function("table", select_overload<uint16_t (fs_typeinfo&)>(
            [](fs_typeinfo& f){ return f.table; }
        ))
        .function("is_dynamic", select_overload<bool (fs_typeinfo&)>(
            [](fs_typeinfo& f){ return f.is_dynamic; }
        ))
        .function("size", select_overload<uint16_t (fs_typeinfo&)>(
            [](fs_typeinfo& f){ return f.size; }
        ))
        .function("info", select_overload<fs_info* (fs_typeinfo&)>(
            [](fs_typeinfo& f){ return f.info; }
        ), allow_raw_pointer<arg<0>>());

    class_<fs>("fs")
        .property("properties", &fs::properties)
        .property("name", &fs::name)
        .property("hash", &fs::hash)
        .function("info", select_overload<fs_info* (fs&)>(
            [](fs& f){ return f.info; }
        ), allow_raw_pointer<arg<0>>())
        .function("get", select_overload<fs* (fs&, int)>(
            [](fs& f, int i) -> fs* {
                if ( f.info->dynamic && f.properties.size() == 1 ) {
                    return &f.properties[0];
                }

                if (i >= f.properties.size()) {
                    return nullptr;
                }

                return &f.properties[i];
            }
        ), allow_raw_pointer<arg<0>>());

    class_<flattened_serializer>("flattened_serializer")
        .function("spew", &flattened_serializer::spew)
        .function("spew_string", select_overload<std::string (flattened_serializer*, int)>(
            [](flattened_serializer* fs, int i) {
                std::stringstream s("");
                fs->spew(i, s);
                return s.str();
            }
        ), allow_raw_pointer<arg<0>>())
        .function("get", &flattened_serializer::get)
        .function("get_otype", &flattened_serializer::get_otype, allow_raw_pointer<arg<0>>());

    /// ----------------------------------------------------------------
    /// parser.hpp
    /// ----------------------------------------------------------------

    class_<parser>("parser")
        .constructor<>()
        .property("buildnumber", &parser::buildnumber)
        .property("entity_classes", &parser::classes)
        .function("class_name", select_overload<std::string (parser&, uint32_t)>(
            [](parser& p, uint32_t idx) {
                return p.classes.classes.by_index(idx).key;
            }
        ))
        .function("serializers", select_overload<flattened_serializer* (parser&)>(
            [](parser& p) {
                return p.serializers;
            }
        ), allow_raw_pointer<arg<0>>())
        .property("particle_manager", &parser::particles)
        .property("baselines", &parser::baselines)
        //.property("entities", &parser::entities)
        .property("events", &parser::events)
        .property("tick", &parser::tick)
        .function("stringtables", select_overload<dict<stringtable>* (parser&)>(
            [](parser& p) -> dict<stringtable>* {
                return &p.stringtables;
            }
        ), allow_raw_pointer<arg<0>>())
        .function("entity", select_overload<jsEntity (parser&, uint32_t i)>(
            [](parser& p, uint32_t i) {
                entity* e = p.entities[i];
                return jsEntity(e);
            }
        ))
        .function("open", select_overload<void (parser&, std::string path, jsVisitor*)>(
            [](parser& p, std::string path, jsVisitor* v) {
                p.open(path.c_str(), v);
            }
        ), allow_raw_pointer<arg<2>>())
        .function("reset", &parser::reset)
        .function("parse", &parser::parse, allow_raw_pointer<arg<0>>())
        .function("parse_all", &parser::parse_all, allow_raw_pointer<arg<0>>())
        .function("require", &parser::require)
        .function("seek", &parser::seek);

    enum_<parser::state>("parser_state")
        .value("BEGIN", parser::state::BEGIN)
        .value("SENDTABLES", parser::state::SENDTABLES)
        .value("END", parser::state::END);

    class_<parser::seekinfo>("parser_seekinfo")
        .property("gamestart", &parser::seekinfo::gamestart)
        .property("pregamestart", &parser::seekinfo::pregamestart);

    /// ----------------------------------------------------------------
    /// property.hpp
    /// ----------------------------------------------------------------

    enum_<property::types>("property_type")
        .value("V_BOOL", property::V_BOOL)
        .value("V_INT32", property::V_INT32)
        .value("V_INT64", property::V_INT64)
        .value("V_UINT32", property::V_UINT32)
        .value("V_UINT64", property::V_UINT64)
        .value("V_FLOAT", property::V_FLOAT)
        .value("V_STRING", property::V_STRING)
        .value("V_VECTOR", property::V_VECTOR);

    class_<jsProperty>("property")
        .function("type", &jsProperty::type)
        .function("info", &jsProperty::info, allow_raw_pointer<arg<0>>())
        .function("value", select_overload<val (jsProperty&)>(
            [](jsProperty& p) {
                switch ( p.type() ) {
                case property::V_BOOL:
                    return val(p.data().b);
                // there is no 64-bit support for now
                case property::V_INT32:
                case property::V_INT64:
                    return val(p.data().i32);
                // there is no 64-bit support for now
                case property::V_UINT32:
                case property::V_UINT64:
                    return val(p.data().u32);
                case property::V_FLOAT:
                    return val(p.data().fl);
                case property::V_STRING:
                    return val(p.data_str());
                case property::V_VECTOR: {
                    val rv(val::object());

                    rv.set(0, p.data().vec[0]);
                    rv.set(1, p.data().vec[0]);
                    rv.set(2, p.data().vec[0]);

                    return rv;
                };
                default:
                    return val::null();
                }
            })
        );

    /// ----------------------------------------------------------------
    /// property_decoder.hpp
    /// ----------------------------------------------------------------

    emscripten::function("prop_decoder_name", &prop_decoder_name, allow_raw_pointer<arg<1>>());

    /// ----------------------------------------------------------------
    /// stringtable.hpp
    /// ----------------------------------------------------------------

    class_<stringtable>("stringtable")
        .function("size", &stringtable::size)
        .function("has_key", &stringtable::has_key)
        .function("by_key", &stringtable::by_key)
        .function("has_index", &stringtable::has_index)
        .function("by_index", &stringtable::by_index)
        .function("clear", &stringtable::clear)
        .function("name", &stringtable::name);

    /// ----------------------------------------------------------------
    /// util_chash.hpp
    /// ----------------------------------------------------------------

    emscripten::function("chash", select_overload<uint64_t (std::string)>(
        [](std::string str) {
            return constexpr_hash_rt(str.c_str());
        })
    );

    /// ----------------------------------------------------------------
    /// visitor.hpp
    /// ----------------------------------------------------------------

    enum_<entity_state>("entity_state")
        .value("ENT_CREATED", ENT_CREATED)
        .value("ENT_UPDATED", ENT_UPDATED)
        .value("ENT_DELETED", ENT_DELETED);

    class_<visitor>("visitor")
        .allow_subclass<jsVisitor>("jsVisitor")
        .function("on_packet", optional_override([](visitor& self, uint32_t id, char* data, uint32_t size) {
            return self.visitor::on_packet(id, data,size);
        }), allow_raw_pointer<arg<2>>())
        .function("on_state", optional_override([](visitor& self, uint32_t state) {
            return self.visitor::on_state(state);
        }))
        .function("on_entity", optional_override([](visitor& self, entity_state state, entity* ent) {
            return self.visitor::on_entity(state, ent);
        }), allow_raw_pointer<arg<2>>())
        .function("on_tick", optional_override([](visitor& self, int32_t tick) {
            return self.visitor::on_tick(tick);
        }))
        .function("on_progress", optional_override([](visitor& self, float perc) {
            return self.visitor::on_progress(perc);
        }));
}
