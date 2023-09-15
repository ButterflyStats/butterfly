/**
 * @file bindings.cpp
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
 *    Python bindings for butterfly.
 */

///
/// WARNING: No object life-time managment added to any kind of structure. Will not work outside of very specific debug / test cases. Yet.
///
/// @todo: Particle

#include <butterfly/butterfly.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

using namespace butterfly;
namespace py = pybind11;

namespace butterfly {
    /** Create a binding for a dictionary */
    template <typename T, typename... Args>
    pybind11::class_<dict<T>> bind_dict(pybind11::module &m, std::string const &name, Args&&... args) {
        // Dictionary
        using Dict = dict<T>;
        py::class_<Dict> cl(m, name.c_str(), std::forward<Args>(args)...);

        // Basic functions
        cl.def("size", &Dict::size, "Returns current size of dictionary")
            .def("reserve", &Dict::reserve, "Reserve a minimum number of free slots")
            .def("clear", &Dict::clear, "Erase the dictionary")
            .def("has_key", &Dict::has_key, "Returns true if the given key exists")
            .def("has_index", &Dict::has_index, "Returns true if the given index exists")
            .def("by_key", &Dict::by_key, "Returns an entry by key", py::return_value_policy::reference)
            .def("by_index", &Dict::by_index, "Returns an entry by value", py::return_value_policy::reference);

        // Entry
        using DictEntry = typename dict<T>::entry_t;
        std::string ename = name+"Entry";

        py::class_<DictEntry> cle(cl, ename.c_str());
        cle.def_readonly("index", &DictEntry::index, "Numeric index of entry")
            .def_readonly("key", &DictEntry::key, "Entry key")
            .def_readonly("value", &DictEntry::value, "Entry value");

        cl.def("__getitem__", [](Dict &v, uint32_t i) -> T {
            if (v.has_index(i)) {
                return v.by_index(i).value;
            } else {
                throw pybind11::index_error();
            }
        }, py::return_value_policy::reference);

        cl.def("__getitem__", [](Dict &v, const std::string& key) -> T {
            if (v.has_key(key)) {
                return v.by_key(key).value;
            } else {
                throw pybind11::index_error();
            }
        }, py::return_value_policy::reference);

        cl.def("__len__", &Dict::size);

        cl.def("__iter__", [](Dict &v) {
                return pybind11::make_iterator(v.begin(), v.end());
            }, pybind11::keep_alive<0, 1>()
        );

        cl.def("__contains__", [](Dict &v, const std::string &k) { return v.has_key(k); } );
        cl.def("__contains__", [](Dict &v, uint32_t i) { return v.has_index(i); } );

        return cl;
    }

    /** Create a binding for a templatized ringbuffer */
    template <typename T, typename... Args>
    pybind11::class_<ringbuffer<T>> bind_ringbuffer(pybind11::module &m, std::string const &name, Args&&... args) {
        py::class_<ringbuffer<T>> cl(m, name.c_str(), std::forward<Args>(args)...);

        // Basic ringbuffer
        cl.def(py::init<uint32_t>(), "Create ringbuffer with given size.")
            .def("position", &ringbuffer<T>::position, "Current buffer position")
            .def("max", &ringbuffer<T>::max, "Maximum number of entries")
            .def("size", &ringbuffer<T>::size, "Buffer size")
            .def("resize", &ringbuffer<T>::resize, "Resize the buffer, cannot shrink")
            .def("at", &ringbuffer<T>::at, "Returns element at given position", py::return_value_policy::reference)
            .def("get_data", &ringbuffer<T>::get_data, "Returns a vector containing all the data", py::return_value_policy::reference);

        // Custom insert function
        cl.def("insert",
           [](ringbuffer<T> &r, T &value) { r.insert(value); },
           py::arg("element"), "Insert an element into the ringbuffer");

        return cl;
    }
}

PYBIND11_PLUGIN(butterpy) {
    py::module m("butterpy", "Butterfly Python bindings");

    bind_dict<entity_classes::class_info>(m, "dict_class_info");
    bind_dict<stringtable>(m, "dict_stringtable");
    bind_dict<std::string>(m, "dict_string");

    /// ----------------------------------------------------------------
    /// dem.hpp
    /// ----------------------------------------------------------------

    py::class_<dem_header>(m, "dem_header")
        .def_readonly("headerid", &dem_header::headerid, "Magic header ID field")
        .def_readonly("offset_summary", &dem_header::offset_summary, "Summary to offset packet")
        .def_readonly("offset_2", &dem_header::offset_2, "Unkown offset");

    py::class_<dem_packet>(m, "dem_packet")
        .def_readonly("tick", &dem_packet::tick, "Tick this packet was parsed at, empty for nested packages. Alternativ: demfile->tick")
        .def_readonly("type", &dem_packet::type, "Packet type")
        .def_readonly("size", &dem_packet::size, "Packet size in bytes")
        .def_readonly("data", &dem_packet::data, "Pointer to start of packet data");

    m.def("dem_uncompress", &dem_uncompress, "Decompress a compressed dem packet");
    m.def("dem_from_buffer", &dem_from_buffer, "Reads data from given buffer into msg and returns bytes read", py::arg("msg"),  py::arg("buffer"),  py::arg("buffer_size"), py::arg("read_tick") = false);

    /// ----------------------------------------------------------------
    /// demfile.hpp
    /// ----------------------------------------------------------------

    py::class_<demfile>(m, "demfile")
        .def(py::init<const char*, dem_progress_cb>(), "Create a demfile from the given path", py::arg("path"), py::arg("progress_cb") = nullptr)
        .def(py::init<char*, std::size_t, dem_progress_cb>(), "Create a demfile from a buffer", py::arg("buffer"), py::arg("size"), py::arg("progress_cb") = nullptr)
        .def("get", &demfile::get, "Returns a single DEM packet")
        .def("good", &demfile::good, "Returns whether there is still data left to read")
        .def("pos", &demfile::pos, "Returns the current position in the file")
        .def("set_pos", &demfile::set_pos, "Sets the current position in the file");

    /// ----------------------------------------------------------------
    /// entity_classes.hpp
    /// ----------------------------------------------------------------

    py::class_<entity_classes> py_entity_classes(m, "entity_classes");
    py_entity_classes.def_readonly("data", &entity_classes::classes, "Entity class dictionary")
        .def("is_networked", &entity_classes::is_networked, "Returns true if class is networked")
        .def("class_id", &entity_classes::class_id, "Returns id of class")
        .def("bits", &entity_classes::bits, "Returns number of bits to read for entity classes");

    py::class_<entity_classes::class_info>(py_entity_classes, "class_info")
        .def_readonly("hash", &entity_classes::class_info::hash, "Class name hashed via fnv1a")
        .def_readonly("type", &entity_classes::class_info::type, "Entity type class");

    /// ----------------------------------------------------------------
    /// entity.hpp
    /// ----------------------------------------------------------------

    py::class_<entity> py_entity(m, "entity");
    py_entity.def_readonly("properties", &entity::properties, "Properties, indexes by their hash")
        .def_readonly("id", &entity::id, "Own entity ID in global list")
        .def_readonly("cls_hash", &entity::cls_hash, "Class hash")
        .def("cls", [](entity& e) { return e.cls; }, "Class id")
        .def("type", [](entity& e) { return e.type; }, "Type")
        .def("set_serializer", &entity::set_serializer, "Set reference serialzier")
        .def("parse", &entity::parse, "Parse entity data from bitstream")
        .def("spew", &entity::spew, "Spew property to console")
        .def("has", (bool (entity::*)(const std::string &)) &entity::has, "Returns true if field exists")
        .def("get", (property* (entity::*)(const std::string &)) &entity::get, "Get field by its hash", py::return_value_policy::reference);

    py::enum_<entity_types>(py_entity, "etype")
        .value("ENT_DEFAULT", ENT_DEFAULT)
        .value("ENT_ABILITY", ENT_ABILITY)
        .value("ENT_NPC", ENT_NPC)
        .value("ENT_HERO", ENT_HERO)
        .value("ENT_ITEM", ENT_ITEM)
        .value("ENT_UNIT", ENT_UNIT)
        .export_values();

    /// ----------------------------------------------------------------
    /// eventlist.hpp
    /// ----------------------------------------------------------------

    py::class_<eventlist> py_eventlist(m, "eventlist");
    py_eventlist
        .def("by_name", &eventlist::by_name, "Return event descriptor by name")
        .def("by_id", &eventlist::by_id, "Return event descriptor by id");

    py::class_<eventlist::descriptor> py_eventlist_descr(py_eventlist, "descriptor");
    py_eventlist_descr
        .def_readonly("name", &eventlist::descriptor::name, "Event name")
        .def_readonly("id", &eventlist::descriptor::id, "Event ID")
        .def_readonly("keys", &eventlist::descriptor::keys, "List of event keys");

    py::class_<eventlist::descriptor::key>(py_eventlist_descr, "key")
        .def_readonly("name", &eventlist::descriptor::key::name, "Attribute Name")
        .def_readonly("type", &eventlist::descriptor::key::type, "Attribute Type");

    /// ----------------------------------------------------------------
    /// flattened_serializer.hpp
    /// ----------------------------------------------------------------

    py::class_<fs_info>(m, "fs_info")
        .def_readonly("name", &fs_info::name, "Name")
        .def_readonly("field", &fs_info::field, "Original field index")
        .def_readonly("encoder", &fs_info::encoder, "Encoder Hash")
        .def_readonly("type", &fs_info::type, "Outer type hash")
        .def_readonly("flags", &fs_info::flags, "Encoding flags")
        .def_readonly("min", &fs_info::min, "Min value (float)")
        .def_readonly("max", &fs_info::max, "Max value (float)")
        .def("bits", [](fs_info& f){ return f.bits; }, "Bits used to encode type")
        .def("dynamic", [](fs_info& f){ return f.dynamic; }, "Whether this property is dynamic");

    py::class_<fs_typeinfo>(m, "fs_typeinfo")
        .def("is_table", [](fs_typeinfo& f){ return f.is_table; }, "Whether this property is pointing at another class")
        .def("table", [](fs_typeinfo& f){ return f.table; }, "Index of said table")
        .def("is_dynamic", [](fs_typeinfo& f){ return f.is_dynamic; }, "Whether this property is dynamic")
        .def("size", [](fs_typeinfo& f){ return f.size; }, "Number of members if this is an array")
        .def_readonly("info", &fs_typeinfo::info, "FS Info pointer", py::return_value_policy::reference);

    py::class_<fs> py_fs(m, "fs");
    py_fs.def_readonly("properties", &fs::properties, "List of fields / props")
        .def_readonly("info", &fs::info, "Pointer to field info", py::return_value_policy::reference)
        .def_readonly("name", &fs::name, "FS name")
        .def_readonly("hash", &fs::hash, "FS Hash");

    py_fs.def("__getitem__", [](fs &v, uint32_t i) -> fs& {
        if ( v.info->dynamic && v.properties.size() == 1 ) {
            return v.properties[0];
        }

        if (i >= v.properties.size()) {
            throw pybind11::index_error();
        }

        return v.properties[i];
    }, py::return_value_policy::reference);

    py::class_<flattened_serializer>(m, "flattened_serializer")
        .def("spew", &flattened_serializer::spew, "Dump serializer at given index to console")
        .def("get", &flattened_serializer::get, "Return serialzier at given index", py::return_value_policy::reference);

    /// ----------------------------------------------------------------
    /// parser.hpp
    /// ----------------------------------------------------------------

    py::class_<parser> py_parser(m, "parser");
    py_parser.def_readonly("dem", &parser::dem, "Demfile object")
        .def_readonly("buildnumber", &parser::buildnumber, "Server buildnumber")
        .def_readonly("stringtables", &parser::stringtables, "Stringtable dictionary")
        .def_readonly("entity_classes", &parser::classes, "Dictionary of entity classes")
        .def_readonly("flattened_serializer", &parser::serializers, "Flattened Serializers object")
        .def_readonly("particle_manager", &parser::particles, "Particle Manager object")
        .def_readonly("baselines", &parser::baselines, "List of entity baselines, indexed by their class id")
        .def_readonly("entities", &parser::entities, "Global entity list")
        .def_readonly("events", &parser::events, "Eventlist object")
        .def_readonly("tick", &parser::tick, "The current tick")
        .def(py::init<bool>(), "Create a new parser", py::arg("enable_seeking") = false)

        .def("open", [](parser& p, const char* path, std::function<void (float)> cb) -> void {
            typedef void function_t( float );
            function_t* ptr_fn = cb.target<function_t>();
            if (ptr_fn) {
                p.open(path, ptr_fn);
            } else {
                p.open(path, nullptr);
            }
        }, "Read a demofile from the given path")

        .def("reset", &parser::reset, "Reset the parser state")
        .def("parse", &parser::parse, "Parse a single packet")
        .def("parse_all", &parser::parse_all, "Parse all packets")
        .def("require", &parser::require, "Enabled forwarding of given packet id")
        .def("seek", &parser::seek, "Seek to the given second in the replay")
        .def("seek_info", &parser::seek_info, "Returns seeking information", py::return_value_policy::reference);

    py::enum_<parser::state>(py_parser, "state")
        .value("BEGIN", parser::state::BEGIN)
        .value("SENDTABLES", parser::state::SENDTABLES)
        .value("END", parser::state::END)
        .export_values();

    py::class_<parser::seekinfo>(py_parser, "seekinfo")
        .def_readonly("gamestart", &parser::seekinfo::gamestart, "Time at which the game starts")
        .def_readonly("pregamestart", &parser::seekinfo::pregamestart, "Time at which the horn sounds");

    /// ----------------------------------------------------------------
    /// particle.hpp
    /// ----------------------------------------------------------------

    /// ----------------------------------------------------------------
    /// property_decoder.hpp
    /// ----------------------------------------------------------------

    /// ----------------------------------------------------------------
    /// property.hpp
    /// ----------------------------------------------------------------

    py::class_<property> py_property(m, "property");
    py_property.def_readonly("type", &property::type, "Property type")
        .def_readonly("info", &property::info, "Pointer to flattened serializer info")
        .def("value", [](property& p) {
            switch ( p.type ) {
            case property::V_BOOL:
                return py::object( py::bool_(p.data.b) );
            case property::V_INT32:
                return py::object( py::int_(p.data.i32) );
            case property::V_INT64:
                return py::object( py::int_(p.data.i64) );
            case property::V_UINT32:
                return py::object( py::int_(p.data.u32) );
            case property::V_UINT64:
                return py::object( py::int_(p.data.u64) );
            case property::V_FLOAT:
                return py::object( py::float_(p.data.fl) );
            case property::V_STRING:
                return py::object( py::str(p.data_str) );
            case property::V_VECTOR: {
                py::list r(3);
                r[0] = py::float_(p.data.vec[0]);
                r[1] = py::float_(p.data.vec[1]);
                r[2] = py::float_(p.data.vec[2]);

                return py::object(r);
            };
            default:
                return py::object( py::str("Unkown") );
            }
        }, "Returns the property value");

    py::enum_<property::types>(py_property, "ptype")
        .value("V_BOOL", property::V_BOOL)
        .value("V_INT32", property::V_INT32)
        .value("V_INT64", property::V_INT64)
        .value("V_UINT32", property::V_UINT32)
        .value("V_UINT64", property::V_UINT64)
        .value("V_FLOAT", property::V_FLOAT)
        .value("V_STRING", property::V_STRING)
        .value("V_VECTOR", property::V_VECTOR)
        .export_values();

    /// ----------------------------------------------------------------
    /// resource_manifest.hpp
    /// ----------------------------------------------------------------

    /// ----------------------------------------------------------------
    /// stringtable.hpp
    /// ----------------------------------------------------------------

    py::class_<stringtable> py_stringtable(m, "stringtable");
    py_stringtable.def("size", &stringtable::size, "Returns number of entries")
        .def("has_key", &stringtable::has_key, "Whether the specific key exists")
        .def("by_key", &stringtable::by_key, "Return entry by key", py::return_value_policy::reference)
        .def("has_index", &stringtable::has_index, "Whether the given index exists")
        .def("by_index", &stringtable::by_index, "Return entry by index", py::return_value_policy::reference)
        .def("clear", &stringtable::clear, "Clear the stringtable")
        .def("name", &stringtable::name, "Name of this stringtable");

    py_stringtable.def("__getitem__", [](stringtable &v, uint32_t i) -> stringtable::value_type {
        if (v.has_index(i)) {
            return v.by_index(i);
        } else {
            throw pybind11::index_error();
        }
    }, py::return_value_policy::reference);

    py_stringtable.def("__getitem__", [](stringtable &v, const std::string& k) -> stringtable::value_type {
        if (v.has_key(k)) {
            return v.by_key(k);
        } else {
            throw pybind11::index_error();
        }
    }, py::return_value_policy::reference);

    py_stringtable.def("__len__", &stringtable::size);

    py_stringtable.def("__iter__", [](stringtable &v) {
            return pybind11::make_iterator(v.begin(), v.end());
        }, pybind11::keep_alive<0, 1>()
    );

    /// ----------------------------------------------------------------
    /// util_bitstream.hpp
    /// ----------------------------------------------------------------

    m.def("bit_at", &bit_at, "Retuns bit mask for single bit at given position");

    py::class_<bitstream>(m, "bitstream")
        .def(py::init<>(), "Create an empty bitstream")
        .def(py::init<const std::string&>(), "Create a bitstream from the given data")
        .def("good", &bitstream::good, "Checks whether there is still data left to be read")
        .def("remaining", &bitstream::remaining, "Returns number of bits left")
        .def("end", &bitstream::end, "Returns the size of the stream in bits")
        .def("position", &bitstream::position, "Returns the current position of the stream in bits")
        .def("set_position", &bitstream::setPosition, "Sets bitstream position")
        .def("seek_forward", &bitstream::seekForward, "Seek n bits forward or until end")
        .def("seek_backward", &bitstream::seekBackward, "Seek n bits back or until the beginning")
        .def("read", &bitstream::read, "Returns result of reading n bits into an uint32_t")
        .def("read_bool", &bitstream::readBool, "Reads a boolean")
        .def("read_varUInt32", &bitstream::readVarUInt32, "Reads a variable sized uint32_t")
        .def("read_varUInt64", &bitstream::readVarUInt64, "Reads a variable sized uint64_t")
        .def("read_varSInt32", &bitstream::readVarSInt32, "Reads a variable sized int32_t")
        .def("read_varSInt64", &bitstream::readVarSInt64, "Reads a variable sized int64_t")
        .def("read_string", &bitstream::readString, "Reads a null-terminated string into the buffer")
        .def("read_bits", &bitstream::readBits, "Reads the exact number of bits into the buffer")
        .def("read_bytes", &bitstream::readBytes, "Reads the exact number of bytes into the buffer")
        .def("read_ubitvar", &bitstream::readUBitVar, "Reads a ubitvar, valve's own variable-length integer encoding")
        .def("read_fieldpath", &bitstream::readFPBitVar, "Reads a fieldpath varint")
        .def("read_coord", &bitstream::readCoord, "Read coord")
        .def("read_angle", &bitstream::readAngle, "Read angle")
        .def("read_normal", &bitstream::readNormal, "Read normlized float")
        .def("read_3bit_normal", &bitstream::read3BitNormal, "Read a normalized 3d vector");

    /// ----------------------------------------------------------------
    /// util_chash.hpp
    /// ----------------------------------------------------------------

    m.def("chash", &constexpr_hash_rt, "Hash a string with fnv1a");

    /// ----------------------------------------------------------------
    /// util_vpk.hpp
    /// ----------------------------------------------------------------

    py::class_<vpk_header>(m, "vpk_header")
        .def_readonly("signature", &vpk_header::signature, "VPK signature")
        .def_readonly("version", &vpk_header::version, "Version, should be 1")
        .def_readonly("size", &vpk_header::size, "Header Size");

    py::class_<vpk_directory_entry>(m, "vpk_directory_entry")
        .def_readonly("crc", &vpk_directory_entry::crc, "CRC32 of file")
        .def_readonly("preload_bytes", &vpk_directory_entry::preload_bytes, "Bytes to preload following header")
        .def_readonly("archive_index", &vpk_directory_entry::archive_index, "Archive with file contents")
        .def_readonly("entry_offset", &vpk_directory_entry::entry_offset, "Entry offset from top of file")
        .def_readonly("entry_size", &vpk_directory_entry::entry_size, "Entry size")
        .def_readonly("terminator", &vpk_directory_entry::terminator, "Terminator, should always equal 0xffff");

    py::class_<vpk_entry>(m, "vpk_entry")
        .def_readonly("folder", &vpk_entry::folder, "Folder file is stored in")
        .def_readonly("name", &vpk_entry::name, "File name")
        .def_readonly("extension", &vpk_entry::extension, "File extension")
        .def_readonly("offset", &vpk_entry::offset, "Offset in archive")
        .def_readonly("size", &vpk_entry::size, "File Size")
        .def_readonly("type", &vpk_entry::type, "File Type according to extension")
        .def_readonly("archive", &vpk_entry::archive, "Archive file is stored in")
        .def_readonly("preload", &vpk_entry::preload, "Number of bytes stored in index file");

    py::class_<vpk> py_vpk(m, "vpk");
    py_vpk.def_readonly("entries", &vpk::entries, "List of VPK entries");

    py::enum_<vpk_entry_type>(py_vpk, "vtype")
        .value("VPK_RES_DEFAULT", VPK_RES_DEFAULT)
        .value("VPK_RES_SND", VPK_RES_SMD)
        .value("VPK_RES_MESH", VPK_RES_MESH)
        .value("VPK_RES_TEX", VPK_RES_TEX)
        .value("VPK_RES_MAT", VPK_RES_MAT)
        .value("VPK_RES_PCF", VPK_RES_PCF)
        .value("VPK_RES_MDL", VPK_RES_MDL)
        .value("VPK_RES_ANIM", VPK_RES_ANIM)
        .value("VPK_RES_TXT", VPK_RES_TXT)
        .export_values();

    m.def("vpk_from_path", &vpk_from_path, "Load VPK from given path. Example path: /Steam/steamapps/common/dota 2 beta/game/dota/pak01");
    m.def("vpk_read", &vpk_read, "Read file from vpk into string");

    /// ----------------------------------------------------------------
    /// util_ztime.hpp
    /// ----------------------------------------------------------------

    m.def("ztime", &getZTime, "Returns high resolution time, useful for performance counters");

    /// ----------------------------------------------------------------
    /// visitor.hpp
    /// ----------------------------------------------------------------

    class pyvisitor : public visitor {
    public:
        void on_packet( uint32_t id, char* data, uint32_t size ) override {
            PYBIND11_OVERLOAD( void, visitor, on_packet, id, data, size );
        }

        void on_state( uint32_t state ) override {
            PYBIND11_OVERLOAD( void, visitor, on_state, state );
        }

        void on_entity( entity_state state, entity* ent ) override {
            PYBIND11_OVERLOAD( void, visitor, on_entity, state, ent );
        }

        void on_tick( int32_t tick ) override {
            PYBIND11_OVERLOAD( void, visitor, on_tick, tick );
        }
    };

    py::class_<visitor, std::unique_ptr<visitor>, pyvisitor> py_pyvisitor(m, "visitor");
    py_pyvisitor.def(py::init<>())
        .def("on_packet", &visitor::on_packet)
        .def("on_state", &visitor::on_state)
        .def("on_entity", &visitor::on_entity)
        .def("on_tick", &visitor::on_tick);

    py::enum_<entity_state>(py_pyvisitor, "etype")
        .value("ENT_CREATED", ENT_CREATED)
        .value("ENT_UPDATED", ENT_UPDATED)
        .value("ENT_DELETED", ENT_DELETED)
        .export_values();

    return m.ptr();
}
