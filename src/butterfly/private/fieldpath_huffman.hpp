/**
 * @file fieldpath_huffman.hpp
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

#ifndef BUTTERFLY_FIELDPATH_HUFFMAN_HPP
#define BUTTERFLY_FIELDPATH_HUFFMAN_HPP

#include <cstdint>
#include <cstring>

#include <butterfly/util_bitstream.hpp>

#include "fieldpath.hpp"
#include "fieldpath_operations.hpp"
#include "util_huffman.hpp"

namespace butterfly {
    /** Field operation type */
    struct fieldop {
        /** Fieldpath name */
        const char* name;
        /** Pointer to operation */
        void ( *fp )( bitstream& b, fieldpath& f );
        /** Weight */
        uint32_t weight;
    };

    /** Global list of fieldpath operations */
    extern std::vector<fieldop> fieldpath_operations;

    /** Fieldop Huffman-Coding type */
    typedef huffman<fieldop> fieldop_huffman;

    /** Static fieldpath lookup */
    force_inline fieldop* fieldop_lookup( uint32_t id ) {
        switch ( id ) {
        case 0: // PlusOne
            return &fieldpath_operations[0];
        case 2: // FieldPathEncodeFinish
            return &fieldpath_operations[39];
        case 14: // PlusTwo
            return &fieldpath_operations[1];
        case 15: // PushOneLeftDeltaNRightNonZeroPack6Bits
            return &fieldpath_operations[11];
        case 24: // PushOneLeftDeltaOneRightNonZero
            return &fieldpath_operations[8];
        case 26: // PlusN
            return &fieldpath_operations[4];
        case 50: // PlusThree
            return &fieldpath_operations[2];
        case 51: // PopAllButOnePlusOne
            return &fieldpath_operations[29];
        case 217: // PushOneLeftDeltaNRightNonZero
            return &fieldpath_operations[10];
        case 218: // PushOneLeftDeltaOneRightZero
            return &fieldpath_operations[7];
        case 220: // PushOneLeftDeltaNRightZero
            return &fieldpath_operations[9];
        case 222: // PopAllButOnePlusNPack6Bits
            return &fieldpath_operations[32];
        case 223: // PlusFour
            return &fieldpath_operations[3];
        case 432: // PopAllButOnePlusN
            return &fieldpath_operations[30];
        case 438: // PushOneLeftDeltaNRightNonZeroPack8Bits
            return &fieldpath_operations[12];
        case 439: // NonTopoPenultimatePlusOne
            return &fieldpath_operations[37];
        case 442: // PopAllButOnePlusNPack3Bits
            return &fieldpath_operations[31];
        case 443: // PushNAndNonTopological
            return &fieldpath_operations[26];
        case 866: // NonTopoComplexPack4Bits
            return &fieldpath_operations[38];
        case 1735: // NonTopoComplex
            return &fieldpath_operations[36];
        case 3469: // PushOneLeftDeltaZeroRightZero
            return &fieldpath_operations[5];
        case 27745: // PopOnePlusOne
            return &fieldpath_operations[27];
        case 27749: // PushOneLeftDeltaZeroRightNonZero
            return &fieldpath_operations[6];
        case 55488: // PopNAndNonTopographical
            return &fieldpath_operations[35];
        case 55489: // PopNPlusN
            return &fieldpath_operations[34];
        case 55492: // PushN
            return &fieldpath_operations[25];
        case 55493: // PushThreePack5LeftDeltaN
            return &fieldpath_operations[24];
        case 55494: // PopNPlusOne
            return &fieldpath_operations[33];
        case 55495: // PopOnePlusN
            return &fieldpath_operations[28];
        case 55496: // PushTwoLeftDeltaZero
            return &fieldpath_operations[13];
        case 110994: // PushThreeLeftDeltaZero
            return &fieldpath_operations[15];
        case 110995: // PushTwoPack5LeftDeltaZero
            return &fieldpath_operations[14];
        case 111000: // PushTwoLeftDeltaN
            return &fieldpath_operations[21];
        case 111001: // PushThreePack5LeftDeltaOne
            return &fieldpath_operations[20];
        case 111002: // PushThreeLeftDeltaN
            return &fieldpath_operations[23];
        case 111003: // PushTwoPack5LeftDeltaN
            return &fieldpath_operations[22];
        case 111004: // PushTwoLeftDeltaOne
            return &fieldpath_operations[17];
        case 111005: // PushThreePack5LeftDeltaZero
            return &fieldpath_operations[16];
        case 111006: // PushThreeLeftDeltaOne
            return &fieldpath_operations[19];
        case 111007: // PushTwoPack5LeftDeltaOne
            return &fieldpath_operations[18];
        default:
            return nullptr;
        }
    }
} /* butterfly */

#endif /* BUTTERFLY_FIELDPATH_HUFFMAN_HPP */
