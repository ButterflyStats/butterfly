/**
 * @file flattened_serializer.hpp
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
 *    Code used to handle Source 2's new flattened serializer structure and related class information.
 */

#ifndef BUTTERFLY_FLATTENED_SERIALIZER_HPP
#define BUTTERFLY_FLATTENED_SERIALIZER_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <unordered_map>

#include <butterfly/proto/netmessages.pb.h>
#include <butterfly/util_assert.hpp>
#include <butterfly/util_noncopyable.hpp>
#include <butterfly/property_decoder.hpp>
#include <butterfly/property_type.hpp>
#include <butterfly/util_bitstream.hpp>
#include <butterfly/entity_classes.hpp>
#include <butterfly/quantized.hpp>

namespace butterfly {
    struct fs;

    /** Field information */
    struct fs_info {
        quantized_float_decoder quantized_decoder;
        /** Name hash */
        uint64_t hash = 0;
        /** Name symbol ID */
        uint16_t name_sym = 0;
        /** Property type */
        PropertyType type = PropertyType::BOOL;

        fs_info( quantized_float_decoder quantized_decoder, uint16_t name_sym, uint64_t hash );

        template <typename T>
        T extract_number(const void* addr) const {
            switch ( type ) {
                case PropertyType::BOOL:
                    return T( *reinterpret_cast<const bool*>( addr ) );
                case PropertyType::INT32:
                    return T( *reinterpret_cast<const int32_t*>( addr ) );
                case PropertyType::INT64:
                    return T( *reinterpret_cast<const int64_t*>( addr ) );
                case PropertyType::UINT32:
                    return T( *reinterpret_cast<const uint32_t*>( addr ) );
                case PropertyType::UINT64:
                    return T( *reinterpret_cast<const uint64_t*>( addr ) );
                case PropertyType::FLOAT:
                    return T( *reinterpret_cast<const float*>( addr ) );
                default:
                    ASSERT_TRUE( false, "Can't cast non-primitive type to number" );
                    return T();
            }
        }
        template <typename T>
        T extract_value( const void* addr ) const;
    };

    enum class PropertyDecoderType : uint8_t {
        NONE,
        ARRAY,
        TABLE,
        TABLE_PTR,
        BOOL,
        FIXED64,
        VARUINT,
        VARINT,
        COORD,
        FLOAT,
        FLOAT_NO_DECODER,
        SIMTIME,
        VECTOR2,
        QANGLE_PITCH_YAW,
        NORMALIZED_VECTOR,
        VECTOR,
        COORD_VECTOR,
        QANGLE,
        QUATERNION,
        STRING,
        RESOURCE,
    };

    /** Type information about a property */
    struct fs_typeinfo {
        /** Normal information */
        const fs_info* info = nullptr;
        /** Class this property is pointing at */
        const fs* table = nullptr;
        /** Is this an array? */
        bool is_array = false;
        /** Decoder type */
        PropertyDecoderType decoder_type = PropertyDecoderType::NONE;

        fs_typeinfo( const fs_info* info );
    };

    class properties;

    struct field_array_container {
        uint8_t* data = nullptr;
        size_t size = 0;
    };

    struct fs_table_data {
        /** List of fields / props */
        std::vector<const fs*> properties;
        /** Mapping of property name hash to property index */
        std::unordered_map<uint64_t, size_t> properties_map;
    };

    /** Flattened serializer table for a single networked class, points to decoder and property */
    struct fs {
        /** Returns field at given position */
        const fs* get_property( uint32_t n ) const {
            if ( is_array() )
                return array_prop;
            ASSERT_TRUE( is_table() && n < table_data->properties.size(), "FS out-of-bounds" );
            return table_data->properties[n];
        }

        force_inline bool is_array() const {
            return decoder_type == PropertyDecoderType::ARRAY;
        }

        force_inline bool is_table() const {
            return decoder_type == PropertyDecoderType::TABLE ||
                   decoder_type == PropertyDecoderType::TABLE_PTR ||
                   decoder_type == PropertyDecoderType::NONE;
        }

        /** Additional table/array data if applicable */
        union {
            const fs_table_data* table_data;
            const fs* array_prop;
        };
        /** Pointer to field info */
        const fs_info* info = nullptr;
        uint32_t offset = 0;
        uint32_t size = 0;
        /** Decoder type */
        PropertyDecoderType decoder_type;
    };

    /**  Flattened serializer structure introduced in Source 2 */
    class flattened_serializer : noncopyable {
    public:
        /** Initialize serializer from data */
        flattened_serializer( uint8_t* data, uint32_t size );

        ~flattened_serializer();

        /** Build serializers for given entity classes */
        void build( const entity_classes& cls );

        /** Return serializer at given index */
        const fs* get( uint32_t idx );

        /** Return name symbol at given index */
        const std::string& get_name( uint32_t name_sym ) const;

    private:
        struct build_ctx {
            std::unordered_map<std::string, const fs*> name2table{};
            std::unordered_map<const fs*, uint8_t> table2alignment{};
        };

        /** Returns metadata for field */
        const fs_typeinfo* get_metadata( build_ctx& ctx, uint32_t field );
        
        uint8_t compute_prop_alignment( build_ctx& ctx, fs* f );
        uint32_t compute_prop_size( build_ctx& ctx, fs* f );

        void cleanup();

        std::vector<fs_info*> infos;
        std::vector<fs_table_data*> tables_data;
        std::vector<fs*> field_serializers;
        std::vector<fs*> class_id2table;
        /** Stores metadata per property */
        std::unordered_map<uint32_t, fs_typeinfo*> metadata{};
        /** Serializer data from replay */
        CSVCMsg_FlattenedSerializer serializers;
    };
} /* butterfly */

#endif /* BUTTERFLY_FLATTENED_SERIALIZER_HPP */
