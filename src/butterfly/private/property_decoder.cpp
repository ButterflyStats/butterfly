/**
 * @file property_decoder.cpp
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

#include <butterfly/flattened_serializer.hpp>
#include <butterfly/property.hpp>
#include <butterfly/resources.hpp>

#include <butterfly/util_bitstream.hpp>
#include <butterfly/util_chash.hpp>
#include "quantized.hpp"

namespace butterfly {
    /* clang-format off */

    const char* prop_decoder_name( fs& f ) {
        if (f.decoder == prop_decode_qangle_pitch_yawn) return "QAngle (Pitch & Yawn)";
        if (f.decoder == prop_decode_bool) return "Bool";
        if (f.decoder == prop_decode_fixed64) return "Int64 (fixed length)";
        if (f.decoder == prop_decode_coord) return "Coord";
        if (f.decoder == prop_decode_dynamic) return "Dynamic Array";
        if (f.decoder == prop_decode_float) return "Float";
        if (f.decoder == prop_decode_simtime) return "Simulation Time";
        if (f.decoder == prop_decode_normal) return "Normal (float)";
        if (f.decoder == prop_decode_noscale) return "Noscale (float)";
        if (f.decoder == prop_decode_quantized) return "Quantized (float)";
        if (f.decoder == prop_decode_vector) return "Vector";
        if (f.decoder == prop_decode_vector2d) return "VectorXY";
        if (f.decoder == prop_decode_qangle) return "QAngle";
        if (f.decoder == prop_decode_string) return "String";
        if (f.decoder == prop_decode_varint) return "Varint";
        if (f.decoder == prop_decode_resource) return "Resource Path";

        return "Unknown";
    }

    void prop_decode_bool( bitstream& b, fs_info* f, property* p ) {
        p->data.b = b.readBool();
        p->type = property::V_BOOL;
    }

    void prop_decode_fixed64( bitstream& b, fs_info* f, property* p ) {
        p->data.u64 = (uint64_t)b.read(32) | ((uint64_t)b.read(32) << 32);
        p->type = property::V_UINT64;
    }

    void prop_decode_coord( bitstream& b, fs_info* f, property* p ) {
        p->data.fl = b.readCoord();
        p->type = property::V_FLOAT;
    }

    void prop_decode_dynamic( bitstream& b, fs_info* f, property* p ) {
        p->data.u64 = b.readVarUInt64();
        p->type = property::V_UINT64;
    }

    void prop_decode_normal( bitstream& b, fs_info* f, property* p ) {
        p->data.vec = b.read3BitNormal();
        p->type = property::V_VECTOR;
    }

    //** Internal inlined version */
    static force_inline float prop_decode_quantized_i( bitstream& b, fs_info* f ) {
        // Leaks as well as breaks multi-usage. @todo: Fix
        static std::vector<quantized_float_decoder*> decs(20480, nullptr);

        if (!decs[f->field]) {
            decs[f->field] = new quantized_float_decoder( f->bits, f->flags, f->min, f->max );
        }

        return decs[f->field]->decode( b );
    }

    void prop_decode_quantized( bitstream& b, fs_info* f, property* p ) {
        p->data.fl = prop_decode_quantized_i( b, f );
        p->type = property::V_FLOAT;
    }

    /** Internal inlined version */
    static force_inline float prop_decode_noscale_i( bitstream& b, fs_info* f ) {
        union { uint32_t i; float f; } u;
        u.i = b.read( 32 );
        return u.f;
    }

    void prop_decode_noscale( bitstream& b, fs_info* f, property* p ) {
        p->data.u32 = b.read(32);
        p->type = property::V_FLOAT;
    }

    /** Internal inlined version */
    static force_inline float prop_decode_float_i( bitstream& b, fs_info* f ) {
        if ( f->encoder == "coord"_chash ) {
            return b.readCoord();
        }

        if ( f->bits == 0 || f->bits >= 32 ) {
            return prop_decode_noscale_i( b, f );
        }

        return prop_decode_quantized_i( b, f );
    }

    void prop_decode_float( bitstream& b, fs_info* f, property* p ) {
        p->data.fl = prop_decode_float_i( b, f );
        p->type = property::V_FLOAT;
    }

    void prop_decode_simtime( bitstream& b, fs_info* f, property* p ) {
        static constexpr float frame_time = (1.0f / 30.0f);
        p->data.fl = b.readVarUInt64() * frame_time;
        p->type = property::V_FLOAT;
    }

    void prop_decode_quaternion( bitstream& b, fs_info* f, property* p ) {
        p->data.quat = std::array<float, 4>{{
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f),
        }};

        p->type = property::V_VECTOR;
    }

    void prop_decode_vector( bitstream& b, fs_info* f, property* p ) {
        p->data.vec = std::array<float, 3>{{
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f)
        }};

        p->type = property::V_VECTOR;
    }

    /** Decode vector2d */
    void prop_decode_vector2d( bitstream& b, fs_info* f, property* p ) {
        p->data.vec = std::array<float, 3>{{
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f),
           0.0f
        }};

        p->type = property::V_VECTOR;
    }

    /** Decode vector4d */
    void prop_decode_vector4d( bitstream& b, fs_info* f, property* p ) {
        p->data.quat = std::array<float, 4>{{
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f),
           prop_decode_float_i(b, f)
        }};

        p->type = property::V_VECTOR;
    }

    void prop_decode_qangle( bitstream& b, fs_info* f, property* p ) {
        if ( f->bits != 0 ) {
            p->data.vec = std::array<float, 3>{{
               prop_decode_float_i(b, f),
               prop_decode_float_i(b, f),
               prop_decode_float_i(b, f)
            }};

            p->type = property::V_VECTOR;
        } else {
            std::array<float, 3> toSet{ {0.0f, 0.0f, 0.0f} };

            bool b1 = b.readBool();
            bool b2 = b.readBool();
            bool b3 = b.readBool();

            // Get reference
            if ( p->type == property::V_VECTOR ) {
                toSet = p->data.vec;
            }

            if ( b1 ) toSet[0] = b.readCoord();
            if ( b2 ) toSet[1] = b.readCoord();
            if ( b3 ) toSet[2] = b.readCoord();

            if ( p->type != property::V_VECTOR ) {
                p->data.vec = toSet;
            }

            p->type = property::V_VECTOR;
        }
    }

    void prop_decode_qangle_pitch_yawn( bitstream& b, fs_info* f, property* p ) {
        p->data.vec = std::array<float, 3>{{
           b.readAngle(f->bits),
           b.readAngle(f->bits),
           0
        }};

        p->type = property::V_VECTOR;
    }

    void prop_decode_string( bitstream& b, fs_info* f, property* p ) {
        char buffer[1024];
        b.readString( buffer, 1024 );
        p->data_str = std::string(buffer);
        p->type = property::V_STRING;
    }

    void prop_decode_varint( bitstream& b, fs_info* f, property* p ) {
        p->data.u64 = b.readVarUInt64();
        p->type = property::V_UINT64;
    }

    void prop_decode_svarint( bitstream& b, fs_info* f, property* p ) {
        p->data.u64 = b.readVarSInt64();
        p->type = property::V_INT64;
    }

    void prop_decode_resource( bitstream& b, fs_info* f, property* p ) {
        uint64_t idx = b.readVarUInt64();

        if (idx == 0) {
            p->data_str = "none";
            p->type = property::V_STRING;
        } else {
            p->data_str = resource_lookup(idx);
            p->type = property::V_STRING;
        }
    }

    /* clang-format on */
} /* butterfly */
