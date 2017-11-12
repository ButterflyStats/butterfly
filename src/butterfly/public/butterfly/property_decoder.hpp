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
    class property;
    struct fs_info;
    struct fs;

    /// Decoder function prototype
    typedef void( decoder_fcn )( bitstream&, fs_info*, property* );

    /** Returns decoder name as string */
    const char* prop_decoder_name( fs& f );

    /** Decode boolean */
    void prop_decode_bool( bitstream& b, fs_info* f, property* p );

    /** Decode integer as 64 bit little endian (fixed-width) */
    void prop_decode_fixed64( bitstream& b, fs_info* f, property* p );

    /** Decode float coord */
    void prop_decode_coord( bitstream& b, fs_info* f, property* p );

    /** Decode size property */
    void prop_decode_dynamic( bitstream& b, fs_info* f, property* p );

    /** Decode float */
    void prop_decode_float( bitstream& b, fs_info* f, property* p );

    /** Decode simulation time */
    void prop_decode_simtime( bitstream& b, fs_info* f, property* p );

    /** Decode normalized float */
    void prop_decode_normal( bitstream& b, fs_info* f, property* p );

    /** Decode standard float */
    void prop_decode_noscale( bitstream& b, fs_info* f, property* p );

    /** Decode quantized */
    void prop_decode_quantized( bitstream& b, fs_info* f, property* p );

    /** Decode Quaternion */
    void prop_decode_quaternion( bitstream& b, fs_info* f, property* p );

    /** Decode vector */
    void prop_decode_vector( bitstream& b, fs_info* f, property* p );

    /** Decode vector2d */
    void prop_decode_vector2d( bitstream& b, fs_info* f, property* p );

    /** Decode angle */
    void prop_decode_qangle( bitstream& b, fs_info* f, property* p );

    /** Decode angle (pitch and yawn only) */
    void prop_decode_qangle_pitch_yawn( bitstream& b, fs_info* f, property* p );

    /** Decode string */
    void prop_decode_string( bitstream& b, fs_info* f, property* p );

    /** Decode standard varint */
    void prop_decode_varint( bitstream& b, fs_info* f, property* p );

    /** Decode standard varint (signed) */
    void prop_decode_svarint( bitstream& b, fs_info* f, property* p );

    /** Decodes resource string */
    void prop_decode_resource( bitstream& b, fs_info* f, property* p );
} /* butterfly */

#endif /* BUTTERFLY_PROPERTY_DECODER_HPP */
