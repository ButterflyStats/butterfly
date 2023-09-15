#ifndef PROPERTY_TYPE_HPP
#define PROPERTY_TYPE_HPP

#include <cstdint>
#include <cstddef>
#include <string>
#include <sstream>
#include <array>

namespace butterfly {
    enum class PropertyType : uint8_t {
        BOOL,
        INT32,
        INT64,
        UINT32,
        UINT64,
        FLOAT,
        STRING,
        VECTOR2,    //< std::array<float, 2>
        VECTOR3,    //< std::array<float, 3>
        QUATERNION  //< std::array<float, 4>
    };

    inline size_t get_property_type_size( PropertyType type ) {
        static_assert( sizeof( float ) == 4, "Float is expected to be a 32-bit type" );
        switch ( type ) {
            case PropertyType::BOOL:
                return 1;
            case PropertyType::INT32:
            case PropertyType::UINT32:
            case PropertyType::FLOAT:
                return 4;
            case PropertyType::INT64:
            case PropertyType::UINT64:
            case PropertyType::VECTOR2:
                return 8;
            case PropertyType::VECTOR3:
                return 12;
            case PropertyType::QUATERNION:
                return 16;
            case PropertyType::STRING:
                return sizeof( std::string );
            default:
                ASSERT_TRUE( false, "" );
        }
    }

    inline std::string property_to_string( const uint8_t* prop_addr, PropertyType type ) {
        switch ( type ) {
        case PropertyType::BOOL:
            return *reinterpret_cast<const bool*>( prop_addr ) ? "true" : "false";
        case PropertyType::INT32:
            return std::to_string( *reinterpret_cast<const int32_t*>( prop_addr ) );
        case PropertyType::INT64:
            return std::to_string( *reinterpret_cast<const int64_t*>( prop_addr ) );
        case PropertyType::UINT32:
            return std::to_string( *reinterpret_cast<const uint32_t*>( prop_addr ) );
        case PropertyType::UINT64:
            return std::to_string( *reinterpret_cast<const uint64_t*>( prop_addr ) );
        case PropertyType::FLOAT:
            return std::to_string( *reinterpret_cast<const float*>( prop_addr ) );
        case PropertyType::STRING:
            return *reinterpret_cast<const std::string*>( prop_addr );
        case PropertyType::VECTOR2: {
            std::stringstream s( "" );
            auto& vec2 = *reinterpret_cast<const std::array<float, 2>*>( prop_addr );
            s << "[" << vec2[0] << ", " << vec2[1] << "]";
            return s.str();
        } break;
        case PropertyType::VECTOR3: {
            std::stringstream s( "" );
            auto& vec3 = *reinterpret_cast<const std::array<float, 3>*>( prop_addr );
            s << "[" << vec3[0] << ", " << vec3[1] << ", " << vec3[2] << "]";
            return s.str();
        } break;
        case PropertyType::QUATERNION: {
            std::stringstream s( "" );
            auto& quat = *reinterpret_cast<const std::array<float, 4>*>( prop_addr );
            s << "[" << quat[0] << ", " << quat[1] << ", " << quat[2] << ", " << quat[3] << "]";
            return s.str();
        } break;
        default:
            return "Unknown";
        }
    }
}

#endif /* PROPERTY_TYPE_HPP */
