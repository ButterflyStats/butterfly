/**
 * @file property.hpp
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

#ifndef BUTTERFLY_PROPERTY_HPP
#define BUTTERFLY_PROPERTY_HPP

#include <array>
#include <string>
#include <sstream>
#include <vector>
#include <cstdint>

#include <butterfly/flattened_serializer.hpp>
#include <butterfly/util_assert.hpp>

namespace butterfly {
    /** Dynamic entity property */
    class property {
    public:
        /** Vector type */
        typedef std::array<float, 3> Vector;

        /** Quaternion type */
        typedef std::array<float, 4> Quaternion;

        /** Different variant types */
        enum types {
            V_BOOL,
            V_INT32,
            V_INT64,
            V_UINT32,
            V_UINT64,
            V_FLOAT,
            V_STRING,
            V_VECTOR,    //< std::array<3, float>
            V_ARRAY,     // Array of properties
            V_QUATERNION // < std::array<4, float>
        };

        /** Property info */
        const fs* info;

        /** Data storage */
        union u {
            bool b;
            uint32_t u32;
            uint64_t u64;
            int32_t i32;
            int64_t i64;
            float fl;
            Vector vec;
            Quaternion quat;
        } data;

        /** Data storage used if type == V_STRING */
        std::string data_str;

        /** Property type */
        uint8_t type;

        /** Constructor */
        property() = default;

        /** Destructor */
        ~property() = default;

        /** Copy constructor */
        property(const property&) = default;

        /** Returns property as string */
        std::string as_string() {
            switch ( type ) {
            case V_BOOL:
                return data.b ? "true" : "false";
            case V_INT32:
                return std::to_string( data.i32 );
            case V_INT64:
                return std::to_string( data.i64 );
            case V_UINT32:
                return std::to_string( data.u32 );
            case V_UINT64:
                return std::to_string( data.u64 );
            case V_FLOAT:
                return std::to_string( data.fl );
            case V_STRING:
                return data_str;
            case V_VECTOR: {
                std::stringstream s( "" );
                s << "[" << data.vec[0] << "|" << data.vec[1] << "|" << data.vec[2] << "]";
                return s.str();
            } break;
            case V_QUATERNION: {
              std::stringstream s( "" );
              s << "[" << data.quat[0] << "|" << data.quat[1] << "|" << data.quat[2] << "|" << data.quat[3] << "]";
              return s.str();
            } break;
            case V_ARRAY:
                return "[Property List]";
            default:
                return "Unkown";
            }
        }
    };
} /* butterfly */

#endif /* BUTTERFLY_PROPERTY_HPP */
