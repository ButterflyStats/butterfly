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
#include <butterfly/resource_manifest.hpp>

#include <butterfly/quantized.hpp>
#include <butterfly/util_bitstream.hpp>
#include <butterfly/util_chash.hpp>

namespace butterfly {
    /* clang-format off */

    const char* prop_decoder_name( fs& f ) {
        if (f.is_array())
            return "Dynamic Array";

        switch (f.decoder_type) {
            case PropertyDecoderType::BOOL:
                return "Bool";
            case PropertyDecoderType::FIXED64:
                return "Int64 (fixed length)";
            case PropertyDecoderType::VARUINT:
                return "Varuint";
            case PropertyDecoderType::VARINT:
                return "Varint";
            case PropertyDecoderType::COORD:
                return "Coord";
            case PropertyDecoderType::FLOAT:
                return "Float";
            case PropertyDecoderType::FLOAT_NO_DECODER:
                return "Raw Float";
            case PropertyDecoderType::SIMTIME:
                return "Simulation Time";
            case PropertyDecoderType::VECTOR2:
                return "VectorXY";
            case PropertyDecoderType::QANGLE_PITCH_YAW:
                return "QAngle (Pitch & Yaw)";
            case PropertyDecoderType::NORMALIZED_VECTOR:
                return "Normalized vector";
            case PropertyDecoderType::VECTOR:
                return "Vector";
            case PropertyDecoderType::COORD_VECTOR:
                return "Coord vector";
            case PropertyDecoderType::QANGLE:
                return "QAngle";
            case PropertyDecoderType::QUATERNION:
                return "Quaternion";
            case PropertyDecoderType::STRING:
                return "String";
            case PropertyDecoderType::RESOURCE:
                return "Resource path string";
            default:
                return "Unknown";
        }
    }

    void prop_decode_bool( bitstream& b, const fs_info* f, void* p ) {
        *reinterpret_cast<bool*>( p ) = b.readBool();
    }

    void prop_decode_fixed64( bitstream& b, const fs_info* f, void* p ) {
        *reinterpret_cast<uint64_t*>( p ) = (uint64_t)b.read(32) | ((uint64_t)b.read(32) << 32);
    }

    void prop_decode_coord( bitstream& b, const fs_info* f, void* p ) {
        *reinterpret_cast<float*>( p ) = b.readCoord();
    }

    void prop_decode_normal( bitstream& b, const fs_info* f, void* p ) {
        *reinterpret_cast<std::array<float, 3>*>( p ) = b.read3BitNormal();
    }

    void prop_decode_float( bitstream& b, const fs_info* f, void* p ) {
        *reinterpret_cast<float*>( p ) = f->quantized_decoder.decode( b );
    }

    void prop_decode_float_no_decoder(bitstream& b, const fs_info* f, void* p ) {
        *reinterpret_cast<uint32_t*>( p ) = b.read(32);
    }

    void prop_decode_simtime( bitstream& b, const fs_info* f, void* p ) {
        constexpr float frame_time = 1.0f / 30.0f;
        *reinterpret_cast<float*>( p ) = b.readVarUInt64() * frame_time;
    }

    void prop_decode_quaternion( bitstream& b, const fs_info* f, void* p ) {
        auto& quat = *reinterpret_cast<std::array<float, 4>*>( p );
        quat[0] = f->quantized_decoder.decode( b );
        quat[1] = f->quantized_decoder.decode( b );
        quat[2] = f->quantized_decoder.decode( b );
        quat[3] = f->quantized_decoder.decode( b );
    }

    void prop_decode_coord_vector( bitstream& b, const fs_info* f, void* p ) {
        auto& vec = *reinterpret_cast<std::array<float, 3>*>( p );
        vec[0] = b.readCoord();
        vec[1] = b.readCoord();
        vec[2] = b.readCoord();
    }

    void prop_decode_vector( bitstream& b, const fs_info* f, void* p ) {
        auto& vec = *reinterpret_cast<std::array<float, 3>*>( p );
        vec[0] = f->quantized_decoder.decode( b );
        vec[1] = f->quantized_decoder.decode( b );
        vec[2] = f->quantized_decoder.decode( b );
    }

    /** Decode vector2d */
    void prop_decode_vector2d( bitstream& b, const fs_info* f, void* p ) {
        auto& vec = *reinterpret_cast<std::array<float, 2>*>( p );
        vec[0] = f->quantized_decoder.decode( b );
        vec[1] = f->quantized_decoder.decode( b );
    }

    void prop_decode_qangle( bitstream& b, const fs_info* f, void* p ) {
        if ( f->quantized_decoder.bit_count() != 0) {
            prop_decode_vector( b, f, p );
        } else {
            bool b1 = b.readBool();
            bool b2 = b.readBool();
            bool b3 = b.readBool();
            
            auto vec = reinterpret_cast<float*>( p );
            if ( b1 ) vec[0] = b.readCoord();
            if ( b2 ) vec[1] = b.readCoord();
            if ( b3 ) vec[2] = b.readCoord();
        }
    }

    void prop_decode_qangle_pitch_yaw( bitstream& b, const fs_info* f, void* p ) {
        auto& vec = *reinterpret_cast<std::array<float, 3>*>( p );
        vec[0] = f->quantized_decoder.decode( b );
        vec[1] = f->quantized_decoder.decode( b );
    }

    void prop_decode_string( bitstream& b, const fs_info* f, void* p ) {
        char buffer[1024];
        size_t size = b.readString( buffer, 1024 );
        reinterpret_cast<std::string*>( p )->assign( buffer, size );
    }

    void prop_decode_varint( bitstream& b, const fs_info* f, void* p ) {
        auto val = b.readVarUInt64();
        switch ( f->type ) {
            case PropertyType::UINT64:
                *reinterpret_cast<uint64_t*>( p ) = val;
                break;
            case PropertyType::UINT32:
                *reinterpret_cast<uint32_t*>( p ) = static_cast<uint32_t>( val );
                break;
            default:
                ASSERT_TRUE( false, "Unexpected prop type" );
        }
    }

    void prop_decode_svarint( bitstream& b, const fs_info* f, void* p ) {
        auto val = b.readVarSInt64();
        switch ( f->type ) {
            case PropertyType::INT64:
                *reinterpret_cast<int64_t*>( p ) = val;
                break;
            case PropertyType::INT32:
                *reinterpret_cast<int32_t*>( p ) = static_cast<int32_t>( val );
                break;
            default:
                ASSERT_TRUE( false, "Unexpected prop type" );
        }
    }

    void prop_decode_resource( const resource_manifest& m, bitstream& b, const fs_info* f, void* p ) {
        uint64_t idx = b.readVarUInt64();

        *reinterpret_cast<std::string*>( p ) = idx != 0
            ? m.resource_lookup(idx)
            : "";
    }

    void DecodeProperty( const resource_manifest& m, PropertyDecoderType dt, bitstream& b, const fs_info* f, void* p ) {
#define DECODE_PROPERTY_HANDLER( type, decoder ) case type: decoder( b, f, p ); break;
        switch ( dt ) {
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::BOOL, prop_decode_bool);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::FIXED64, prop_decode_fixed64);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::VARUINT, prop_decode_varint);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::VARINT, prop_decode_svarint);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::COORD, prop_decode_coord);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::FLOAT, prop_decode_float);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::FLOAT_NO_DECODER, prop_decode_float_no_decoder);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::SIMTIME, prop_decode_simtime);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::VECTOR2, prop_decode_vector2d);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::QANGLE_PITCH_YAW, prop_decode_qangle_pitch_yaw);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::NORMALIZED_VECTOR, prop_decode_normal);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::VECTOR, prop_decode_vector);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::COORD_VECTOR, prop_decode_coord_vector);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::QANGLE, prop_decode_qangle);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::QUATERNION, prop_decode_quaternion);
            DECODE_PROPERTY_HANDLER(PropertyDecoderType::STRING, prop_decode_string);
            case PropertyDecoderType::RESOURCE:
                prop_decode_resource( m, b, f, p );
                break;
            case PropertyDecoderType::TABLE:
            case PropertyDecoderType::ARRAY:
                b.readVarUInt64();
                break;
            case PropertyDecoderType::TABLE_PTR:
                b.readBool();
                break;
            default:
                ASSERT_TRUE( false, "Unknown decoder type" );
        }
#undef DECODE_PROPERTY_HANDLER
    }

    /* clang-format on */
}  // namespace butterfly
