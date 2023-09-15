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

namespace butterfly {
    force_inline void fieldop_lookup( uint32_t id, bitstream& b, fieldpath& fp, bool& op_found, bool& finished ) {
        op_found = true;
        finished = false;
        switch (id) {
#define FP_CASE(id, func) case id: func(b, fp); break
            FP_CASE(0, fp_PlusOne);
            case 2: // fp_FieldPathEncodeFinish
                finished = true;
                break;
            FP_CASE(14, fp_PlusTwo);
            FP_CASE(15, fp_PushOneLeftDeltaNRightNonZeroPack6Bits);
            FP_CASE(24, fp_PushOneLeftDeltaOneRightNonZero);
            FP_CASE(26, fp_PlusN);
            FP_CASE(50, fp_PlusThree);
            FP_CASE(51, fp_PopAllButOnePlusOne);
            FP_CASE(217, fp_PushOneLeftDeltaNRightNonZero);
            FP_CASE(218, fp_PushOneLeftDeltaOneRightZero);
            FP_CASE(220, fp_PushOneLeftDeltaNRightZero);
            FP_CASE(222, fp_PopAllButOnePlusNPack6Bits);
            FP_CASE(223, fp_PlusFour);
            FP_CASE(432, fp_PopAllButOnePlusN);
            FP_CASE(438, fp_PushOneLeftDeltaNRightNonZeroPack8Bits);
            FP_CASE(439, fp_NonTopoPenultimatePlusOne);
            FP_CASE(442, fp_PopAllButOnePlusNPack3Bits);
            FP_CASE(443, fp_PushNAndNonTopological);
            FP_CASE(866, fp_NonTopoComplexPack4Bits);
            FP_CASE(1735, fp_NonTopoComplex);
            FP_CASE(3469, fp_PushOneLeftDeltaZeroRightZero);
            FP_CASE(27745, fp_PopOnePlusOne);
            FP_CASE(27749, fp_PushOneLeftDeltaZeroRightNonZero);
            FP_CASE(55488, fp_PopNAndNonTopological);
            FP_CASE(55489, fp_PopNPlusN);
            FP_CASE(55492, fp_PushN);
            FP_CASE(55493, fp_PushThreePack5LeftDeltaN);
            FP_CASE(55494, fp_PopNPlusOne);
            FP_CASE(55495, fp_PopOnePlusN);
            FP_CASE(55496, fp_PushTwoLeftDeltaZero);
            FP_CASE(110994, fp_PushThreeLeftDeltaZero);
            FP_CASE(110995, fp_PushTwoPack5LeftDeltaZero);
            FP_CASE(111000, fp_PushTwoLeftDeltaN);
            FP_CASE(111001, fp_PushThreePack5LeftDeltaOne);
            FP_CASE(111002, fp_PushThreeLeftDeltaN);
            FP_CASE(111003, fp_PushTwoPack5LeftDeltaN);
            FP_CASE(111004, fp_PushTwoLeftDeltaOne);
            FP_CASE(111005, fp_PushThreePack5LeftDeltaZero);
            FP_CASE(111006, fp_PushThreeLeftDeltaOne);
            FP_CASE(111007, fp_PushTwoPack5LeftDeltaOne);
#undef FP_CASE
            default:
                op_found = false;
                break;
        }
    }
} /* butterfly */

#endif /* BUTTERFLY_FIELDPATH_HUFFMAN_HPP */
