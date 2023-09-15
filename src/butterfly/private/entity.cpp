/**
 * @file entity.cpp
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

#include <vector>
#include <cstdint>
#include <iostream>

#include <butterfly/entity.hpp>
#include <butterfly/flattened_serializer.hpp>
#include <butterfly/util_bitstream.hpp>
#include <butterfly/util_chash.hpp>
#include <butterfly/parser.hpp>

#include "config_internal.hpp"
#include "fieldpath.hpp"
#include "fieldpath_huffman.hpp"
#include "fieldpath_operations.hpp"
#include "util_ascii_table.hpp"

namespace butterfly {
    entity::entity( const fs* ser ) : ser( ser ), properties( new uint8_t[ser->size]{} ) {
        init_props( properties, ser );
    }

    void entity::init_props( uint8_t* ptr, const fs* f ) {
        if ( f->is_table() )
            for ( auto prop : f->table_data->properties ) {
                auto prop_addr = &ptr[prop->offset];
                init_props( prop_addr, prop );
            }
        else if ( !f->is_array() && f->info->type == PropertyType::STRING )
            new ( ptr ) std::string();
    }
    
    entity::~entity() {
        deinit_props( properties, ser );
        delete[] properties;
    }
    
    void entity::deinit_props( uint8_t* ptr, const fs* f ) {
        if ( f->is_array() ) {
            auto prop = f->array_prop;
            auto& ar = *reinterpret_cast<field_array_container*>( ptr );
            for ( size_t i = 0; i < ar.size; i++ )
                deinit_props( &ar.data[i * prop->size], prop );
            delete[] ar.data;
        } else if ( f->is_table() )
            for ( auto prop : f->table_data->properties ) {
                auto prop_addr = &ptr[prop->offset];
                deinit_props( prop_addr, prop );
            }
        else if ( f->info->type == PropertyType::STRING )
            reinterpret_cast<std::string*>( ptr )->~basic_string();
    }

    entity::entity( const entity& e ) : ser( e.ser ), properties( new uint8_t[ser->size]{} ) {
        this->id = e.id;
        this->cls = e.cls;
        this->type = e.type;
        this->cls_hash = e.cls_hash;
        
        memcpy( properties, e.properties, ser->size );
        copy_props( e.properties, properties, ser );
   }
       
    void entity::copy_props( uint8_t* old_ptr, uint8_t* new_ptr, const fs* f ) {
        if ( f->is_array() ) {
            auto prop = f->array_prop;
            auto& old_ar = *reinterpret_cast<field_array_container*>( old_ptr );
            auto& new_ar = *reinterpret_cast<field_array_container*>( new_ptr );
            new_ar.data = new uint8_t[new_ar.size * prop->size];
            memcpy( new_ar.data, old_ar.data, new_ar.size * prop->size );
            for ( size_t i = 0; i < new_ar.size; i++ ) {
                auto off = i * prop->size;
                copy_props( &new_ar.data[off], &old_ar.data[off], prop );
            }
        } else if ( f->is_table() ) {
            for ( auto prop : f->table_data->properties ) {
                auto prop_addr_old = &old_ptr[prop->offset];
                auto prop_addr_new = &new_ptr[prop->offset];
                copy_props( prop_addr_old, prop_addr_new, prop );
            }
        }else if ( f->info->type == PropertyType::STRING ) {
            auto& old_str = *reinterpret_cast<std::string*>( old_ptr );
            new ( new_ptr ) std::string( old_str );
        }
    }
    
    void entity::move_props( uint8_t* old_ptr, uint8_t* new_ptr, const fs* f ) {
        if ( f->is_table() ) {
            for ( auto prop : f->table_data->properties ) {
                auto prop_addr_old = &old_ptr[prop->offset];
                auto prop_addr_new = &new_ptr[prop->offset];
                move_props( prop_addr_old, prop_addr_new, prop );
            }
        } else if ( !f->is_array() && f->info->type == PropertyType::STRING ) {
            auto& old_str = *reinterpret_cast<std::string*>( old_ptr );
            new ( new_ptr ) std::string( std::move( old_str ) );
        }
    }

    void entity::parse( const resource_manifest& m, bitstream& b ) {
#if BUTTERFLY_THREADSAFE
        std::lock_guard<std::mutex> lock( mut );
#endif /* BUTTERFLY_THREADSAFE */

        static std::vector<fieldpath> fieldpaths( 1024 );

        fieldpaths.clear();

        {
            fieldpath fp;
            fp.push_back( -1 );

            while ( true ) {
                // Read op
                bool op_found = false;
                bool finished = false;

                for ( uint32_t i = 0, id = 0; !op_found && i < 17; ++i ) {
                    id = ( id << 1 ) | b.readBool();
                    fieldop_lookup( id, b, fp, op_found, finished );
                }

                ASSERT_TRUE( op_found, "Exhausted max operation bits" );

                if ( finished )
                    break;

                ASSERT_GREATER( fp.size(), 0, "Invalid fieldpath size" );
                fieldpaths.push_back( fp );
            }
        }

        for ( auto& fp : fieldpaths ) {
            auto prop_fs = ser->get_property(fp[0]);
            auto prop_addr = &properties[prop_fs->offset];
            for ( size_t i = 1; i < fp.size(); i++ ) {
                bool in_array = prop_fs->is_array();
                prop_fs = prop_fs->get_property(fp[i]);
                if ( in_array ) {
                    auto& ar = *reinterpret_cast<field_array_container*>( prop_addr );
                    if ( ar.size <= fp[i] ) {
                        auto new_size = static_cast<size_t>( fp[i] ) + 1;
                        auto new_data = new uint8_t[new_size * prop_fs->size]{};
                        if ( ar.data != nullptr ) {
                            memcpy( new_data, ar.data, ar.size * prop_fs->size );
                            for ( size_t i = 0; i < ar.size; i++ ) {
                                auto off = i * prop_fs->size;
                                move_props( &ar.data[off], &new_data[off], prop_fs );
                            }
                        }
                        delete[] ar.data;
                        for ( size_t i = ar.size; i < new_size; i++ )
                            init_props( &new_data[i * prop_fs->size], prop_fs );
                        ar.data = new_data;
                        ar.size = new_size;
                    }
                    prop_addr = &ar.data[fp[i] * prop_fs->size];
                } else
                    prop_addr = &prop_addr[prop_fs->offset];
            }
            DecodeProperty( m, prop_fs->decoder_type, b, prop_fs->info, prop_addr );
        }
    }

    void spew_internal(
        ascii_table& tbl, std::string base_path,
        const flattened_serializer* serializers,
        const uint8_t* root_prop_addr,
        const fs* root_prop_info
    ) {
        if ( root_prop_info->is_array() ) {
            auto& root_prop = *reinterpret_cast<const field_array_container*>( root_prop_addr );
            auto prop_info = root_prop_info->array_prop;
            for ( size_t i = 0; i < root_prop.size; i++ ) {
                auto prop_addr = &root_prop.data[i * prop_info->size];
                std::string name = base_path + "[" + std::to_string( i ) + "]";
                if ( !prop_info->is_array() && !prop_info->is_table() )
                    tbl.append( std::move( name ), property_to_string( prop_addr, prop_info->info->type ) );
                else
                    spew_internal( tbl, std::move( name ), serializers, prop_addr, prop_info );
            }
        } else if ( root_prop_info->is_table() ) {
            for ( auto prop_info : root_prop_info->table_data->properties ) {
                auto prop_addr = &root_prop_addr[prop_info->offset];
                auto& prop_name = serializers->get_name( prop_info->info->name_sym );
                std::string name = base_path + "." + prop_name;
                if ( !prop_info->is_array() && !prop_info->is_table() )
                    tbl.append( std::move( name ), property_to_string( prop_addr, prop_info->info->type ) );
                else
                    spew_internal( tbl, std::move( name ), serializers, prop_addr, prop_info );
            }
        }
    }

    void entity::spew( const flattened_serializer* serializers, std::ostream& out ) {
        ascii_table tbl;
        tbl.append( "Key", "Value" );

        for ( size_t i = 0; i < ser->table_data->properties.size(); i++ ) {
            auto prop_info = ser->table_data->properties[i];
            auto prop_addr = &properties[prop_info->offset];
            auto& name = serializers->get_name( prop_info->info->name_sym );
            if ( !prop_info->is_array() && !prop_info->is_table() )
                tbl.append( name, property_to_string( prop_addr, prop_info->info->type ) );
            spew_internal( tbl, name, serializers, prop_addr, prop_info );
        }

        tbl.print( {1, 1}, out );
    }
} /* butterfly */
