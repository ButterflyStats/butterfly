/**
 * @file util_platform.hpp
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

#ifndef BUTTERFLY_UTIL_STRING_HPP
#define BUTTERFLY_UTIL_STRING_HPP

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>
#include <cstdio>
#include <locale>

#include <butterfly/util_assert.hpp>

namespace butterfly {
    /** Strip whitespaces from string start */
    inline std::string& ltrim( std::string& s ) {
        s.erase( s.begin(), std::find_if( s.begin(), s.end(), [](int c) { return std::isspace(c) == 0; } ) );
        return s;
    }

    /** Strip whitespaces from string end */
    inline std::string& rtrim( std::string& s ) {
        s.erase(
            std::find_if( s.rbegin(), s.rend(), [](int c) { return std::isspace(c) == 0; } ).base(), s.end() );
        return s;
    }

    /** Strip whitespaces from both start and end */
    inline std::string& trim( std::string& s ) { return ltrim( rtrim( s ) ); }

    /** Simple string replacement */
    inline void replace_all( std::string& subject, std::string search, std::string rep ) {
        ASSERT_TRUE( search.length() >= 1, "Infinite replace loop detected" );

        size_t pos = 0;
        while ( ( pos = subject.find( search, pos ) ) != std::string::npos ) {
            subject.replace( pos, search.length(), rep );
            pos += rep.length();
        }
    }

    /** Reads a 0 terminated string from a file buffer */
    inline char* fread_nstr( FILE* fp, uint32_t max_len = 64 ) {
        char* ret    = new char[max_len];
        uint32_t pos = 0;

        char c = fgetc( fp );
        if (c == -1) {
            delete[] ret;
            return nullptr;
        }

        while ( c != 0 && pos < max_len ) {
            ret[pos++] = c;
            c          = fgetc( fp );
        }

        if ( pos == 0 ) {
            delete[] ret;
            return nullptr;
        }

        ASSERT_TRUE( pos != max_len, "String doesn't fit buffer" );
        ret[pos] = '\0';
        return ret;
    }
} /* butterfly */

#endif /* BUTTERFLY_UTIL_STRING_HPP */
