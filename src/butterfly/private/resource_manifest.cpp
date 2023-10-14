#include <mutex>
#include <string>

#include <butterfly/resource_manifest.hpp>
#include <butterfly/util_bitstream.hpp>
#include <butterfly/util_platform.hpp>

#include "util_lzss.hpp"
#include "util_murmur.hpp"

namespace butterfly {
    uint32_t insert_or_get_index( std::vector<std::string>& vec,
                                  const char* str, size_t str_size ) {
        auto data = vec.data();
        auto size = vec.size();
        for ( size_t i = 0; i < size; i++ )
            if ( data[i].size() == str_size &&
                 memcmp( data[i].c_str(), str, str_size ) == 0 )
                return i;
        vec.emplace_back( str, str_size );
        return size;
    }

    void fixup_resource_path( char* path, size_t path_len ) {
        for ( size_t i = 0; i < path_len; i++ )
            path[i] = path[i] != '\\' ? tolower( path[i] ) : '/';
    }

    void resource_manifest::update( const std::string& serialized_manifest ) {
        std::string data;
        {  // decompress / extract data
            bitstream b( serialized_manifest );
            const bool isCompressed = b.readBool();
            const uint32_t size      = b.read( 24 );
            ASSERT_LESS_EQ(
                size, b.remaining() / 8,
                "Invalid data size provided in serialized manifest" );
            data.resize( size );
            b.readBytes( &data[0], size );
            if ( isCompressed ) {
                auto size = LZSS_GetActualSize(
                    reinterpret_cast<const uint8_t*>( data.data() ),
                    data.size() );
                ASSERT_LESS( size, 16 * 1024 * 1024,
                             "Compressed manifest is too big" );
                std::string decompressed_data;
                decompressed_data.resize( size );
                decompressed_data.resize( LZSS_Uncompress(
                    reinterpret_cast<const uint8_t*>( data.data() ),
                    data.size(),
                    reinterpret_cast<uint8_t*>( &decompressed_data[0] ),
                    decompressed_data.size() ) );
                data = std::move( decompressed_data );
            }
        }

        bitstream b( data );

        const uint16_t nTypes   = b.read( 16 );
        const uint16_t nDirs    = b.read( 16 );
        const uint16_t nEntries = b.read( 16 );

        auto nTypesBits = required_bits( nTypes ),
             nDirsBits  = required_bits( nDirs );

        char buf[512];

        std::vector<uint32_t> types( nTypes );
        for ( uint16_t i = 0; i < nTypes; i++ ) {
            size_t buf_size = b.readString( buf, sizeof( buf ) );
            fixup_resource_path( buf, buf_size );
            types[i] = insert_or_get_index( this->types, buf, buf_size );
        }

        std::vector<uint32_t> dirs( nDirs );
        for ( uint16_t i = 0; i < nDirs; i++ ) {
            size_t buf_size = b.readString( buf, sizeof( buf ) );
            fixup_resource_path( buf, buf_size );
            dirs[i] = insert_or_get_index( this->directories, buf, buf_size );
        }

        for ( uint16_t i = 0; i < nEntries; i++ ) {
            const uint32_t dir_id = b.read( nDirsBits );
            ASSERT_LESS( dir_id, dirs.size(), "Invalid directory index" );
            auto& dir = dirs[dir_id];

            size_t buf_size = b.readString( buf, sizeof( buf ) );
            fixup_resource_path( buf, buf_size );
            auto file = insert_or_get_index( this->file_names, buf, buf_size );

            const uint32_t type_id = b.read( nTypesBits );
            ASSERT_LESS( type_id, types.size(), "Invalid type index" );
            auto& ext = types[type_id];

            auto path_size = snprintf(
                buf, sizeof( buf ), "%s%s.%s", this->directories[dir].c_str(),
                this->file_names[file].c_str(), this->types[ext].c_str() );
            ASSERT_GREATER( path_size, 0, "" );
            auto path_hash = MurmurHash64( buf, path_size, 0xedabcdef );
            path_hash_to_path_parts[path_hash] = path_parts{ dir, file, ext };
        }
    }

    std::string resource_manifest::resource_lookup( uint64_t hash ) const {
        auto found = path_hash_to_path_parts.find( hash );
        if ( found == path_hash_to_path_parts.end() )
            return "";

        auto& parts = found->second;
        return directories[parts.dir_id] + file_names[parts.file_name_id] +
               "." + types[parts.type_id];
    }
}  // namespace butterfly
