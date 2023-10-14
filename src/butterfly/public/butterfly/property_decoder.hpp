/**
 * @file property_decoder.hpp
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

#ifndef BUTTERFLY_PROPERTY_DECODER_HPP
#define BUTTERFLY_PROPERTY_DECODER_HPP

namespace butterfly {
    /// Forward decl
    class bitstream;
    class resource_manifest;
    struct fs_info;
    struct fs;
    enum class PropertyDecoderType : uint8_t;

    /** Returns decoder name as string */
    const char* prop_decoder_name( fs& f );

    /** Decode boolean */
    void prop_decode_bool( bitstream& b, const fs_info* f, void* p );

    /** Decode integer as 64 bit little endian (fixed-width) */
    void prop_decode_fixed64( bitstream& b, const fs_info* f, void* p );

    /** Decode float coord */
    void prop_decode_coord( bitstream& b, const fs_info* f, void* p );

    /** Decode float */
    void prop_decode_float( bitstream& b, const fs_info* f, void* p );

    /** Decode standard float */
    void prop_decode_float_no_decoder(bitstream& b, const fs_info* f, void* p);

    /** Decode simulation time */
    void prop_decode_simtime( bitstream& b, const fs_info* f, void* p );

    /** Decode normalized float */
    void prop_decode_normal( bitstream& b, const fs_info* f, void* p );

    /** Decode Quaternion */
    void prop_decode_quaternion( bitstream& b, const fs_info* f, void* p );

    /** Decode coord vector */
    void prop_decode_coord_vector( bitstream& b, const fs_info* f, void* p );

    /** Decode vector */
    void prop_decode_vector( bitstream& b, const fs_info* f, void* p );

    /** Decode vector2d */
    void prop_decode_vector2d( bitstream& b, const fs_info* f, void* p );

    /** Decode angle */
    void prop_decode_qangle( bitstream& b, const fs_info* f, void* p );

    /** Decode angle (pitch and yaw only) */
    void prop_decode_qangle_pitch_yaw( bitstream& b, const fs_info* f, void* p );

    /** Decode string */
    void prop_decode_string( bitstream& b, const fs_info* f, void* p );

    /** Decode standard varint */
    void prop_decode_varint( bitstream& b, const fs_info* f, void* p );

    /** Decode standard varint (signed) */
    void prop_decode_svarint( bitstream& b, const fs_info* f, void* p );

    /** Decodes resource string */
    void prop_decode_resource( const resource_manifest& m, bitstream& b, const fs_info* f, void* p );

    void DecodeProperty( const resource_manifest& m, PropertyDecoderType dt, bitstream& b, const fs_info* f, void* p );
} /* butterfly */

#endif /* BUTTERFLY_PROPERTY_DECODER_HPP */
