/**
 * @file flattened_serializer.cpp
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

#include <string>
#include <cstdint>
#include <cstdio>

#include <butterfly/util_assert.hpp>
#include <butterfly/entity_classes.hpp>
#include <butterfly/flattened_serializer.hpp>
#include <butterfly/property_decoder.hpp>
#include <butterfly/util_chash.hpp>
#include <butterfly/util_ztime.hpp>

#include "util_ascii_table.hpp"

namespace butterfly {
    flattened_serializer::flattened_serializer( uint8_t* data, uint32_t size ) {
        ASSERT_TRUE( serializers.ParseFromArray( data, size ), "Unable to parse buffer as FlattenedSerializer packet" );
    }

    flattened_serializer::~flattened_serializer() {
        for ( auto& m : metadata ) {
            delete m.second.info;
        }

        for ( auto& t : tables ) {
            delete t.info;
        }
    }

    /** Return serializer at given index */
    const fs& flattened_serializer::get( uint32_t idx ) { return tables.at( idx ); }

    /* clang-format off */
    void flattened_serializer::build( entity_classes& cls ) {
        BENCHMARK_START(flattened_serializer);

        uint32_t tbl_id = 0;
        tables.resize( cls.classes.size() );

        for ( auto& ser : serializers.serializers() ) {
            // append version to serializer name
            std::string tblName = ( ser.serializer_version() == 0 )
                ? serializers.symbols( ser.serializer_name_sym() )
                : serializers.symbols( ser.serializer_name_sym() ) + std::to_string( ser.serializer_version() );

            // field we are currently parsing
            fs current;

            // iterate all field indicies
            for ( auto& f : ser.fields_index() ) {
                fs field;
                auto info = get_metadata(f);

                if (info.is_table)
                    field = tables_internal.by_index(info.table).value;

                field.decoder = info.decoder;
                field.info = info.info;

                if (info.size) {
                    fs arr = field;

                    if (info.is_dynamic) {
                        fs arr2;
                        arr2.info = info.info;
                        arr2.decoder = prop_decode_dynamic;
                        for (uint32_t i = 0; i < info.size; ++i) {
                            arr2.properties.push_back(arr);
                            arr2.properties.back().name = std::to_string(i);
                        }

                        current.properties.push_back(arr2);
                    } else {
                        for (uint32_t i = 0; i < info.size; ++i) {
                            arr.properties.push_back(field);
                            arr.properties.back().name = std::to_string(i);
                        }

                        current.properties.push_back(arr);
                    }
                } else {
                    current.properties.push_back(field);
                }
            }

            // append to internal table
            tables_internal.insert(tbl_id++, tblName, current);

            // If the entity is networked, also append it to the public table
            if (cls.is_networked(tblName)) {
                current.info = new fs_info{tblName};
                tables[cls.class_id(tblName)] = current;
            }
        }

        // append name and hash
        for (fs& f : tables) {
            for (fs& f2 : f.properties) {
                app_name_hash(f.name, f2);
            }
        }

        BENCHMARK_END(flattened_serializer);
    }

    void flattened_serializer::spew( uint32_t idx, std::ostream& out ) {
        ascii_table tbl;
        tbl.append("Id", "Name", "Type", "Encoder", "Decoder", "Bits", "Flags", "Min", "Max");
        spew_impl(idx, &tbl, "", false);
        tbl.print({1, 1, 1}, out);
    }

    void flattened_serializer::spew_impl( uint32_t idx, void* tbl, std::string target, bool internal ) {
        uint32_t i = 0;
        auto fs = tables[idx];

        if (internal)
            fs = tables_internal.by_index(idx).value;

        for (auto &f : fs.properties) {
            std::string enc = "-";
            if (f.info->encoder != ""_chash) {
                enc = serializers.symbols(serializers.fields(f.info->field).var_encoder_sym());
            }

            ((ascii_table*)tbl)->append(
                target+std::to_string(i),
                f.info->name, serializers.symbols(serializers.fields(f.info->field).var_type_sym()),
                enc, prop_decoder_name(f), f.info->bits, f.info->flags, f.info->min, f.info->max
            );

            if (metadata[f.info->field].is_table && metadata[f.info->field].table != idx) {
                spew_impl(metadata[f.info->field].table, tbl, target+std::to_string(i)+"/", true);
            }

            ++i;
        }
    }

    std::string flattened_serializer::get_otype( fs_info* f ) {
        return serializers.symbols(serializers.fields(f->field).var_type_sym());
    }

    fs_typeinfo& flattened_serializer::get_metadata( uint32_t field ) {
        // Check if we cached the metadata
        auto it = metadata.find( field );
        if ( it != metadata.end() )
            return it->second;

        fs_typeinfo ret{0, 0, 0, 0, 0};

        // Get proto data
        auto& f = serializers.fields( field );

        std::string f_name = serializers.symbols( f.var_name_sym() );
        std::string f_type = serializers.symbols( f.var_type_sym() );
        std::string f_encoder = f.has_var_encoder_sym() ? serializers.symbols( f.var_encoder_sym() ) : "";
        float f_max = f.has_high_value() ? f.high_value() : 1.0f;
        float f_min = f.has_low_value() ? f.low_value() : 0.0f;
        uint32_t f_bc = f.bit_count();
        uint32_t f_flags = f.encode_flags();
        uint64_t h_type  = constexpr_hash_rt(f_type.c_str());
        uint64_t h_encoder  = constexpr_hash_rt(f_encoder.c_str());

        // There are 175 unique type signatures at the moment (28.02.2016).
        // Instead of relying on regex or something similar, we can switch on most of them.

        // Checks if we have a subtable
        if (f.has_field_serializer_name_sym()) {
            ret.is_table = true;

            std::string tblName = (f.field_serializer_version() == 0)
                ? serializers.symbols(f.field_serializer_name_sym())
                : serializers.symbols(f.field_serializer_name_sym()) + std::to_string(f.field_serializer_version());

            ASSERT_TRUE(tables_internal.has_key(tblName), "Unkown serializer requested");
            ret.table = tables_internal.by_key(tblName).index;
        } else {
            ret.is_table = false;
        }

        switch (h_type) {
            #include "flattened_serializer.inline"
            default: {
                printf("%s\n", f_type.c_str());
                ASSERT_TRUE(0 != 0, "Unhandled fs type");
            } break;
        }

        // Check for specific decoders
        switch (h_encoder) {
        case ""_chash:
            break;
        case "coord"_chash:
            if (ret.decoder != prop_decode_vector) {
                ret.decoder = prop_decode_coord;
            }
            break;
        case "fixed64"_chash:
            ret.decoder = prop_decode_fixed64;
            break;
        case "normal"_chash:
            ret.decoder = prop_decode_normal;
            break;
        case "qangle_pitch_yaw"_chash:
            ret.decoder = prop_decode_qangle_pitch_yawn;
            break;
        }

        // Check for simulation time
        switch (constexpr_hash_rt(f_name.c_str())) {
            case "m_flSimulationTime"_chash:
            case "m_flAnimTime"_chash:
                ret.decoder = prop_decode_simtime;
                break;
        }

        // Fill fs_info
        ret.info = new fs_info{f_name, field, h_encoder, h_type, f_bc, ret.is_dynamic, f_flags, f_min, f_max};

        metadata[field] = ret;
        return metadata[field];
    }

    void flattened_serializer::app_name_hash(std::string n, fs& f) {
        if (n.size() > 0) {
            if (f.name.size() == 0) {
                f.name = n.append(".").append(f.info->name);
            } else {
                f.name = n.append(".").append(f.name);
            }
        } else {
            if (f.name.size() == 0) {
                f.name = f.info->name;
            }
        }

        f.hash = constexpr_hash_rt(f.name.c_str());

        for (fs& f2 : f.properties) {
            app_name_hash(f.name, f2);
        }
    }
    /* clang-format on */
} /* butterfly */
