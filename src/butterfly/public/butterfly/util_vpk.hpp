/**
 * @file util_vpk.hpp
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

#ifndef BUTTERFLY_UTIL_VPK_HPP
#define BUTTERFLY_UTIL_VPK_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace butterfly {
#pragma pack( push, 1 )
    /** VPK header, see https://developer.valvesoftware.com/wiki/VPK_File_Format */
    struct vpk_header {
        /** Signature, should equal 0x55aa1234 */
        uint32_t signature;
        /** Version, should equal 1 or 2 */
        uint32_t version;
        /** The size, in bytes, of the directory tree */
        uint32_t size;
    };

    /** Directory entry, internal */
    struct vpk_directory_entry {
        /** A 32bit CRC of the file's data. */
        uint32_t crc;
        /** The number of bytes contained in the index file. */
        uint16_t preload_bytes;

        /**
         * A zero based index of the archive this file's data is contained in.
         * If 0x7fff, the data follows the directory.
         */
        uint16_t archive_index;

        /**
         * If ArchiveIndex is 0x7fff, the offset of the file data relative to the end of the directory (see the header
         * for more details).
         * Otherwise, the offset of the data from the start of the specified archive.
         */
        int32_t entry_offset;

        /**
         * If zero, the entire file is stored in the preload data.
         * Otherwise, the number of bytes stored starting at EntryOffset.
         */
        uint32_t entry_size;

        /** Terminator, should be 0xffff */
        uint16_t terminator;
    };
#pragma pack( pop )

    /** VPK entry types, based on their file extension */
    enum vpk_entry_type {
        VPK_RES_DEFAULT = 0,
        VPK_RES_SMD     = 1,
        VPK_RES_MESH    = 2,
        VPK_RES_TEX     = 3,
        VPK_RES_MAT     = 4,
        VPK_RES_PCF     = 5,
        VPK_RES_MDL     = 6,
        VPK_RES_ANIM    = 7,
        VPK_RES_TXT     = 8
    };

    /** A single vpk entry */
    struct vpk_entry {
        /** Entry Folder */
        char* folder;
        /** Entry Name */
        char* name;
        /** Entry extension */
        char* extension;
        /** Offset */
        uint32_t offset;
        /** Size */
        uint32_t size;
        /** Entry type */
        uint16_t type;
        /** Archive */
        uint16_t archive;
        /** Number of bytes in index file */
        uint16_t preload;
    };

    /** Data for a single vpk file */
    struct vpk {
        /** List of VPK entries */
        std::vector<vpk_entry> entries;
        /** List of pointers to free */
        std::vector<char*> _freelist;
        /** Path to vpk archives */
        const char* path;

        /** Free strings allocated during vpk creation */
        ~vpk() {
            for ( char* p : _freelist ) {
                delete[] p;
            }
        }
    };

    /** Load VPK from given path. Example path: /Steam/steamapps/common/dota 2 beta/game/dota/pak01 */
    vpk* vpk_from_path( const char* path );

    /** Read file from vpk into string */
    std::string vpk_read( vpk* v, const char* folder, const char* name, const char* extension );
}

#endif /* BUTTERFLY_UTIL_VPK_HPP */
