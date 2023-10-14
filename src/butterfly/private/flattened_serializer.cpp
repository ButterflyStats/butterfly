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
#include <butterfly/flattened_serializer.hpp>
#include <butterfly/property_decoder.hpp>
#include <butterfly/util_chash.hpp>
#include <butterfly/util_ztime.hpp>

#include "alloc.hpp"

#include "util_ascii_table.hpp"

namespace butterfly {
    fs_info::fs_info( quantized_float_decoder quantized_decoder, uint16_t name_sym, uint64_t hash )
        : quantized_decoder( std::move( quantized_decoder ) ), hash( hash ), name_sym( name_sym ) {}

    template <>
    bool fs_info::extract_value( const void* addr ) const {
        switch ( type ) {
            case PropertyType::BOOL:
                return *reinterpret_cast<const bool*>( addr );
            case PropertyType::INT32:
                return *reinterpret_cast<const int32_t*>( addr ) != 0;
            case PropertyType::INT64:
                return *reinterpret_cast<const int64_t*>( addr ) != 0;
            case PropertyType::UINT32:
                return *reinterpret_cast<const uint32_t*>( addr ) != 0;
            case PropertyType::UINT64:
                return *reinterpret_cast<const uint64_t*>( addr ) != 0;
            case PropertyType::FLOAT:
                return *reinterpret_cast<const float*>( addr ) != 0.f;
            default:
                ASSERT_TRUE( false, "Can't cast non-primitive type to bool" );
                return false;
        }
    }

    template <>
    float fs_info::extract_value( const void* addr ) const {
        return extract_number<float>( addr );
    }

    template <>
    int32_t fs_info::extract_value( const void* addr ) const {
        return extract_number<int32_t>( addr );
    }

    template <>
    int64_t fs_info::extract_value( const void* addr ) const {
        return extract_number<int64_t>( addr );
    }

    template <>
    uint32_t fs_info::extract_value( const void* addr ) const {
        return extract_number<int32_t>( addr );
    }

    template <>
    uint64_t fs_info::extract_value( const void* addr ) const {
        return extract_number<int64_t>( addr );
    }

    template <>
    std::string fs_info::extract_value( const void* addr ) const {
        ASSERT_TRUE( type == PropertyType::STRING,
                     "Can't cast other types to string" );
        return *reinterpret_cast<const std::string*>( addr );
    }

    template <>
    std::array<float, 2> fs_info::extract_value( const void* addr ) const {
        ASSERT_TRUE( type == PropertyType::VECTOR2,
                     "Can't cast other types to VECTOR2" );
        return *reinterpret_cast<const std::array<float, 2>*>( addr );
    }

    template <>
    std::array<float, 3> fs_info::extract_value( const void* addr ) const {
        ASSERT_TRUE( type == PropertyType::VECTOR3,
                     "Can't cast other types to VECTOR3" );
        return *reinterpret_cast<const std::array<float, 3>*>( addr );
    }

    template <>
    std::array<float, 4> fs_info::extract_value( const void* addr ) const {
        ASSERT_TRUE( type == PropertyType::QUATERNION,
                     "Can't cast other types to QUATERNION" );
        return *reinterpret_cast<const std::array<float, 4>*>( addr );
    }

    fs_typeinfo::fs_typeinfo( const fs_info* info ) : info(info) {}

    bool starts_with( const std::string& str1, std::string str2 ) {
        return str1.size() >= str2.size() && memcmp( str1.data(), str2.data(), str2.size() ) == 0;
    }

    flattened_serializer::flattened_serializer( uint8_t* data, uint32_t size ) {
        ASSERT_TRUE( serializers.ParseFromArray( data, size ), "Unable to parse buffer as FlattenedSerializer packet" );
    }

    flattened_serializer::~flattened_serializer() {
        cleanup();
    }

    /** Return serializer at given index */
    const fs* flattened_serializer::get( uint32_t idx ) {
        ASSERT_LESS( idx, this->class_id2table.size(), "Invalid serializer index provided" );
        return class_id2table[idx];
    }

    /** Return name symbol at given index */
    const std::string& flattened_serializer::get_name( uint32_t name_sym ) const {
        ASSERT_LESS( name_sym, this->serializers.symbols_size(), "Invalid name symbol provided" );
        return serializers.symbols(name_sym);
    }

    uint8_t flattened_serializer::compute_prop_alignment( build_ctx& ctx, fs* f ) {
        static_assert( sizeof( float ) == 4, "Float is expected to be a 32-bit type" );
        if ( f->is_array() )
            return sizeof( void* );
        if ( f->is_table() ) {
            auto found = ctx.table2alignment.find(f);
            if ( found != ctx.table2alignment.end() )
                return found->second;

            uint8_t alignment = 1;
            for ( auto prop_ : f->table_data->properties ) {
                auto prop = const_cast<fs*>( prop_ );
                auto al = compute_prop_size( ctx, prop );
                if ( al != 0 ) {
                    alignment = std::max( alignment, compute_prop_alignment( ctx, prop ) );
                }
            }

            ctx.table2alignment.insert( std::make_pair( f, alignment ) );
            return alignment;
        }
        switch ( f->info->type ) {
            case PropertyType::BOOL:
                return 1;
            case PropertyType::INT32:
            case PropertyType::UINT32:
            case PropertyType::FLOAT:
            case PropertyType::VECTOR2:
            case PropertyType::VECTOR3:
            case PropertyType::QUATERNION:
                return 4;
            case PropertyType::INT64:
            case PropertyType::UINT64:
                return 8;
            case PropertyType::STRING:
                return sizeof( void* );
            default:
                ASSERT_TRUE( false, "" );
                return 1;
        }
    }

    uint32_t flattened_serializer::compute_prop_size( build_ctx& ctx, fs* f ) {
        if (f->is_array()) {
            compute_prop_size( ctx, const_cast<fs*>( f->array_prop ) );
            return sizeof(field_array_container);
        }
        if ( f->is_table() ) {
            if ( f->size != 0 )
                return f->size;

            for ( auto prop_ : f->table_data->properties ) {
                auto prop = const_cast<fs*>( prop_ );
                auto al = compute_prop_alignment( ctx, prop );
                if ( ( f->size % al ) != 0 )
                    f->size += al - ( f->size % al );
                prop->offset = f->size;
                f->size += compute_prop_size( ctx, prop );
            }

            auto al = compute_prop_alignment( ctx, f );
            if ( ( f->size % al ) != 0 )
                f->size += al - ( f->size % al );

            return f->size;
        }
        if ( f->size == 0 )
            f->size = get_property_type_size( f->info->type );
        return f->size;
    }

    /* clang-format off */
    void flattened_serializer::build( const entity_classes& cls ) {
        BENCHMARK_START(flattened_serializer);

        cleanup();
        this->class_id2table.resize(cls->size());

        build_ctx ctx;
        for (auto& ser : this->serializers.serializers()) {
            auto serializer_name_sym = (uint16_t)ser.serializer_name_sym();
            std::string tblName = this->serializers.symbols(serializer_name_sym);
            auto tblNameHash = constexpr_hash_rt(tblName.c_str());
            // append version to serializer name
            if (ser.serializer_version() != 0)
                tblName += std::to_string(ser.serializer_version());

            // field we are currently parsing
            auto current = g_fsalloc.malloc();
            auto current_info = g_fsinfoalloc.malloc(
                quantized_float_decoder{ 0, 0, 0.f, 0.f },
                serializer_name_sym,
                tblNameHash
            );
            auto current_table_data = g_fstabledataalloc.malloc();
            current->info = current_info;
            current->decoder_type = PropertyDecoderType::TABLE;
            current->table_data = current_table_data;
            ctx.name2table[tblName] = current;
            
            tables_data.push_back( current_table_data );
            infos.push_back( current_info );
            field_serializers.push_back( current );
            
            current_table_data->properties_map.reserve( ser.fields_index().size() );
            current_table_data->properties.reserve( ser.fields_index().size() );

            // iterate all field indicies
            for (auto& f : ser.fields_index()) {
                auto field = g_fsalloc.malloc();
                field_serializers.push_back(field);

                auto info = this->get_metadata(ctx, f);

                if (info->table != nullptr)
                    *field = *info->table;

                field->decoder_type = info->decoder_type;
                field->info = info->info;

                if (info->is_array) {
                    auto arr = g_fsalloc.malloc();
                    field_serializers.push_back(arr);

                    arr->info = info->info;
                    arr->decoder_type = PropertyDecoderType::ARRAY;
                    arr->array_prop = field;

                    field = arr;
                }
                
                #if BUTTERFLY_DEVCHECKS
                auto it = current->properties_map.find( field->info->hash );
                // Any hash collision means type access might be screwed
                ASSERT_TRUE( it == current->properties_map.end(), "Hash collision in property set" );
                #endif /* BUTTERFLY_DEVCHECKS */

                current_table_data->properties_map.insert( std::make_pair( field->info->hash, current_table_data->properties.size() ) );
                current_table_data->properties.push_back( field );
            }

            // If the entity is networked, also append it to the public table
            if (cls.is_networked(tblName)) {
                auto class_id = cls.class_id(tblName);
                this->class_id2table[class_id] = current;
            }
        }

        for ( auto f : field_serializers )
            if ( f->is_table() )
                compute_prop_size( ctx, f );

        BENCHMARK_END(flattened_serializer);
    }

    void ReplaceString( std::string& str, std::string from, std::string to ) {
        size_t index = 0;
        while (true) {
            /* Locate the substring to replace. */
            index = str.find( from, index );
            if (index == std::string::npos)
                break;

            /* Make the replacement. */
            str.replace( index, from.size(), to );
        }
    }

    const fs_typeinfo* flattened_serializer::get_metadata( build_ctx& ctx, uint32_t field ) {
        // Check if we cached the metadata
        auto it = metadata.find(field);
        if (it != metadata.end())
            return it->second;

        // Get proto data
        auto& f = this->serializers.fields(field);

        auto f_name_sym = (uint16_t)f.var_name_sym();
        std::string f_name = this->serializers.symbols(f_name_sym);
        std::string f_type = this->serializers.symbols(f.var_type_sym());
        std::string f_encoder = f.has_var_encoder_sym() ? this->serializers.symbols(f.var_encoder_sym()) : "";

        auto ret_info = g_fsinfoalloc.malloc(
            quantized_float_decoder{
                (uint8_t)f.bit_count(),
                (uint8_t)f.encode_flags(),
                f.has_low_value() ? f.low_value() : 0.f,
                f.has_high_value() ? f.high_value() : 1.f
            },
            f_name_sym,
            constexpr_hash_rt(f_name.c_str())
        );
        infos.push_back(ret_info);
        auto ret = g_fstypeinfoalloc.malloc(ret_info);

        if ( f_type.back() == ']' ) {
            ret->is_array = true;
            f_type.erase(f_type.find("["));
        }

        const bool is_ptr = f_type.back() == '*';
        while ( f_type.back() == '*' )
            f_type.pop_back();

        ReplaceString(f_type, "< ", "<");
        ReplaceString(f_type, " >", ">");

        // There are 175 unique type signatures at the moment (28.02.2016).
        // Instead of relying on regex or something similar, we can switch on most of them.

        // Checks if we have a subtable
        if (f.has_field_serializer_name_sym()) {
            std::string tblName = this->serializers.symbols(f.field_serializer_name_sym());
            if (f.field_serializer_version() != 0)
                tblName += std::to_string(f.field_serializer_version());

            auto found = ctx.name2table.find(tblName);
            ASSERT_TRUE(found != ctx.name2table.end(), "");
            ret->table = found->second;
        }

#define VECTOR_TYPE(name) do { \
    if ( starts_with( f_type, #name"<" ) ) { \
        ret->is_array = true; \
        f_type = f_type.substr( sizeof( #name"<" ) - 1, f_type.size() - sizeof( #name"<>" ) + 1 ); \
    } \
} while (false)
        VECTOR_TYPE(CUtlVector);
        VECTOR_TYPE(CNetworkUtlVectorBase);
        VECTOR_TYPE(CUtlVectorEmbeddedNetworkVar);
#undef VECTOR_TYPE

        if ( f_type == "char" ) {
            ret->decoder_type = PropertyDecoderType::STRING;
            ret_info->type = PropertyType::STRING;
            ret->is_array = false;

            metadata[field] = ret;
            return ret;
        }

        if ( starts_with( f_type, "CHandle<" ) ) {
            ret->decoder_type = PropertyDecoderType::VARUINT;
            ret_info->type = PropertyType::UINT32;

            metadata[field] = ret;
            return ret;
        }

        if ( starts_with( f_type, "CStrongHandle<" ) ) {
            ret->decoder_type = PropertyDecoderType::RESOURCE;
            ret_info->type = PropertyType::STRING;

            metadata[field] = ret;
            return ret;
        }

        if (!is_ptr)
            switch ( constexpr_hash_rt( f_type.c_str() ) ) {
                #include "flattened_serializer.inc"
                default:
                    ret->decoder_type = ret->table != nullptr
                        ? PropertyDecoderType::TABLE
                        : PropertyDecoderType::VARUINT;
                    ret_info->type = PropertyType::UINT64;
                    break;
            }
        else {
            ASSERT_TRUE( ret->table != nullptr, "Pointer points to a primitive type" );
            ret->decoder_type = PropertyDecoderType::TABLE_PTR;
            ret_info->type = PropertyType::BOOL;
        }

        // Check for specific decoders
        switch ( constexpr_hash_rt( f_encoder.c_str() ) ) {
            case "coord"_chash:
                switch ( ret->decoder_type ) {
                    case PropertyDecoderType::FLOAT:
                        ret->decoder_type = PropertyDecoderType::COORD;
                        break;
                    case PropertyDecoderType::VECTOR:
                        ret->decoder_type = PropertyDecoderType::COORD_VECTOR;
                        break;
                    default:
                        ASSERT_TRUE( false, "coord decoder applied to an unknown decoder type" );
                        break;
                }
                break;
            case "fixed64"_chash:
                ret->decoder_type = PropertyDecoderType::FIXED64;
                ret_info->type = PropertyType::UINT64;
                break;
            case "normal"_chash:
                ret->decoder_type = PropertyDecoderType::NORMALIZED_VECTOR;
                ret_info->type = PropertyType::VECTOR3;
                break;
            case "qangle_pitch_yaw"_chash:
                ret->decoder_type = PropertyDecoderType::QANGLE_PITCH_YAW;
                ret_info->type = PropertyType::VECTOR3;
                break;
            default:
                break;
        }

        // Check for simulation time
        if ( f_name == "m_flSimulationTime" || f_name == "m_flAnimTime" ) {
            ret->decoder_type = PropertyDecoderType::SIMTIME;
            ret_info->type = PropertyType::FLOAT;
        }

        metadata[field] = ret;
        return ret;
    }

    void flattened_serializer::cleanup() {
        for ( auto& t : tables_data )
            g_fstabledataalloc.free( t );
        tables_data.clear();

        for ( auto& t : field_serializers )
            g_fsalloc.free( t );
        field_serializers.clear();
        class_id2table.clear();

        for ( auto& m : metadata )
            g_fstypeinfoalloc.free( m.second );
        metadata.clear();

        for ( auto& o : infos )
            g_fsinfoalloc.free( o );
        infos.clear();
    }
    /* clang-format on */
} /* butterfly */
