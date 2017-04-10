/**
 * @file fieldpath.hpp
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

#ifndef BUTTERFLY_FIELDPATH_HPP
#define BUTTERFLY_FIELDPATH_HPP

#include <cstdint>
#include <cstdio>
#include <vector>

namespace butterfly {
    /** Type for a source 2 fieldpath */
    struct fieldpath {
        fieldpath() : data( {-1} ), finished( false ) { data.reserve( 6 ); }

        /** Fieldpath indicies */
        std::vector<int32_t> data;
        /** Marks last path in header */
        bool finished;

        /** Reset fieldpath */
        void reset() {
            data.clear();
            data.push_back( -1 );
            finished = false;
        }

        /** Dump fieldpath */
        void spew() {
            /* clang-format off */
            auto& d = data;
            switch ( d.size() ) {
                case 1: printf("%d\n", d[0]); break;
                case 2: printf("%d/%d\n", d[0], d[1]); break;
                case 3: printf("%d/%d/%d\n", d[0], d[1], d[2]); break;
                case 4: printf("%d/%d/%d/%d\n", d[0], d[1], d[2], d[3]); break;
                case 5: printf("%d/%d/%d/%d/%d\n", d[0], d[1], d[2], d[3], d[4]); break;
                case 6: printf("%d/%d/%d/%d/%d/%d\n", d[0], d[1], d[2], d[3], d[4], d[5]); break;
                default: printf("Invalid Fieldpath\n");
            }
            /* clang-format on */
        }
    };
} /* butterfly */

#endif /* BUTTERFLY_FIELDPATH_HPP */
