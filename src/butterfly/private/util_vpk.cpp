/**
 * @file util_vpk.cpp
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

#include <memory>
#include <cstring>
#include <cstdio>

#include <butterfly/util_assert.hpp>
#include <butterfly/util_chash.hpp>
#include <butterfly/util_vpk.hpp>
#include "util_string.hpp"

namespace butterfly {
    static uint8_t vpk_extention_to_type( const char* ext ) {
        switch ( constexpr_hash_rt( ext ) ) {
        case "smd"_chash:
            return VPK_RES_SMD;
        case "vmesh_c"_chash:
            return VPK_RES_MESH;
        case "vtex_c"_chash:
            return VPK_RES_TEX;
        case "vmat_c"_chash:
            return VPK_RES_MAT;
        case "vpcf_c"_chash:
            return VPK_RES_PCF;
        case "vmdl_c"_chash:
            return VPK_RES_MDL;
        case "vanim_c"_chash:
            return VPK_RES_ANIM;
        case "cfg"_chash:
        case "txt"_chash:
            return VPK_RES_TXT;
        default:
            return VPK_RES_DEFAULT;
        }
    }

    vpk* vpk_from_path( const char* path ) {
        std::unique_ptr<vpk> ret( new vpk );

        // construct correct path
        std::string pstart( path );
        std::string pdir = path + std::string( "_dir.vpk" );
        ret->path        = path;

        // open vpk
        FILE* fp = fopen( pdir.c_str(), "r" );
        ASSERT_TRUE( fp, "Unable to read vpk" );

        // read and verify header
        vpk_header header;
        ASSERT_TRUE( fread( &header, sizeof( vpk_header ), 1, fp ) == 1, "Failed to read hader" );

        ASSERT_TRUE( header.signature == 0x55aa1234, "VPK signature mismatch" );
        ASSERT_TRUE( header.version == 1, "Header version mismatch" );

        char* name;
        char* extension;
        char* folder;

        // read entries
        while ( true ) {
            // file extension
            extension = fread_nstr( fp, 16 );
            if ( !extension )
                break;
            ret->_freelist.push_back( extension );

            uint8_t ltype = vpk_extention_to_type( extension );

            while ( true ) {
                // path
                folder = fread_nstr( fp, 192 );
                if ( !folder )
                    break;
                ret->_freelist.push_back( folder );

                while ( true ) {
                    // name
                    name = fread_nstr( fp, 128 );
                    if ( !name )
                        break;
                    ret->_freelist.push_back( name );

                    // actual file information
                    vpk_directory_entry entry;
                    ASSERT_TRUE( fread( &entry, sizeof( vpk_directory_entry ), 1, fp ) == 1, "Failed to read directory entry" );

                    vpk_entry meta;
                    meta.folder    = folder;
                    meta.name      = name;
                    meta.extension = extension;
                    meta.preload   = entry.preload_bytes;
                    meta.archive   = entry.archive_index;
                    meta.size      = entry.entry_size;
                    meta.offset    = entry.entry_offset;
                    meta.type      = ltype;

                    if ( entry.archive_index == 0x7FFF ) {
                        if ( entry.entry_offset < 0 )
                            entry.entry_offset = 0;

                        meta.offset = ftell( fp ) + entry.entry_offset + entry.preload_bytes;
                        fseek( fp, entry.entry_offset + entry.entry_size + entry.preload_bytes, SEEK_CUR );
                    } else if ( entry.preload_bytes > 0 ) {
                        fseek( fp, entry.preload_bytes, SEEK_CUR );
                    }

                    ret->entries.push_back( std::move( meta ) );
                }
            }
        }

        fclose( fp );

        return ret.release();
    }

    std::string vpk_read( vpk* v, const char* folder, const char* name, const char* extension ) {
        ASSERT_TRUE( v && folder && name && extension, "Not accepting null params" );
        std::string ret;

        for ( auto& e : v->entries ) {
            if ( strcmp( folder, e.folder ) != 0 || strcmp( name, e.name ) != 0 ||
                 strcmp( extension, e.extension ) != 0 )
                continue;

            char path[2048];
            snprintf( path, 2048, "%s_%03d.vpk", v->path, e.archive );

            // Open archive
            FILE* fp = fopen( path, "rb" );
            ASSERT_TRUE( fp, "Unable to open archive" );
            ASSERT_TRUE( ( !e.preload ) && ( e.archive != 0x7FFF ), "Preload files not supported" );

            fseek( fp, e.offset, SEEK_CUR );
            ret.resize( e.size );
            ASSERT_TRUE( fread( &ret[0], 1, e.size, fp ), "Failed to read entry" );
            fclose( fp );
        }

        return ret;
    }
}
