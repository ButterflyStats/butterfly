/**
 * @file fieldpath_huffman.cpp
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

#include "util_huffman.hpp"
#include "fieldpath.hpp"
#include "fieldpath_operations.hpp"
#include "fieldpath_huffman.hpp"

namespace butterfly {
    /* clang-format off */
    std::vector<fieldop> fieldpath_operations = {
		{"PlusOne", fp_PlusOne, 36271}, // 0
		{"PlusTwo", fp_PlusTwo, 10334},
		{"PlusThree", fp_PlusThree, 1375},
		{"PlusFour", fp_PlusFour, 646},
		{"PlusN", fp_PlusN, 4128},
		{"PushOneLeftDeltaZeroRightZero", fp_PushOneLeftDeltaZeroRightZero, 35}, // 5
		{"PushOneLeftDeltaZeroRightNonZero", fp_PushOneLeftDeltaZeroRightNonZero, 3},
		{"PushOneLeftDeltaOneRightZero", fp_PushOneLeftDeltaOneRightZero, 521},
		{"PushOneLeftDeltaOneRightNonZero", fp_PushOneLeftDeltaOneRightNonZero, 2942},
		{"PushOneLeftDeltaNRightZero", fp_PushOneLeftDeltaNRightZero, 560},
		{"PushOneLeftDeltaNRightNonZero", fp_PushOneLeftDeltaNRightNonZero, 471}, // 10
		{"PushOneLeftDeltaNRightNonZeroPack6Bits", fp_PushOneLeftDeltaNRightNonZeroPack6Bits, 10530},
		{"PushOneLeftDeltaNRightNonZeroPack8Bits", fp_PushOneLeftDeltaNRightNonZeroPack8Bits, 251},
		{"PushTwoLeftDeltaZero", fp_PushTwoLeftDeltaZero, 1},
		{"PushTwoPack5LeftDeltaZero", fp_PushTwoPack5LeftDeltaZero, 1},
		{"PushThreeLeftDeltaZero", fp_PushThreeLeftDeltaZero, 1}, // 15
		{"PushThreePack5LeftDeltaZero", fp_PushThreePack5LeftDeltaZero, 1},
		{"PushTwoLeftDeltaOne", fp_PushTwoLeftDeltaOne, 1},
		{"PushTwoPack5LeftDeltaOne", fp_PushTwoPack5LeftDeltaOne, 1},
		{"PushThreeLeftDeltaOne", fp_PushThreeLeftDeltaOne, 1},
		{"PushThreePack5LeftDeltaOne", fp_PushThreePack5LeftDeltaOne, 1}, // 20
		{"PushTwoLeftDeltaN", fp_PushTwoLeftDeltaN, 1},
		{"PushTwoPack5LeftDeltaN", fp_PushTwoPack5LeftDeltaN, 1},
		{"PushThreeLeftDeltaN", fp_PushThreeLeftDeltaN, 1},
		{"PushThreePack5LeftDeltaN", fp_PushThreePack5LeftDeltaN, 1},
		{"PushN", fp_PushN, 1}, // 25
		{"PushNAndNonTopological", fp_PushNAndNonTopological, 310},
		{"PopOnePlusOne", fp_PopOnePlusOne, 2},
		{"PopOnePlusN", fp_PopOnePlusN, 1},
		{"PopAllButOnePlusOne", fp_PopAllButOnePlusOne, 1837},
		{"PopAllButOnePlusN", fp_PopAllButOnePlusN, 149}, // 30
		{"PopAllButOnePlusNPack3Bits", fp_PopAllButOnePlusNPack3Bits, 300},
		{"PopAllButOnePlusNPack6Bits", fp_PopAllButOnePlusNPack6Bits, 634},
		{"PopNPlusOne", fp_PopNPlusOne, 1},
		{"PopNPlusN", fp_PopNPlusN, 1},
		{"PopNAndNonTopographical", fp_PopNAndNonTopographical, 1}, // 35
		{"NonTopoComplex", fp_NonTopoComplex, 76},
		{"NonTopoPenultimatePlusOne", fp_NonTopoPenultimatePlusOne, 271},
		{"NonTopoComplexPack4Bits", fp_NonTopoComplexPack4Bits, 99},
		{"FieldPathEncodeFinish", fp_FieldPathEncodeFinish, 25474} // 39
	};
    /* clang-format on */

    /** Fieldop Huffman comparison function */
    template <>
    int32_t huffman_compare<fieldop>( HuffmanNode* left, HuffmanNode* right ) {
        if ( left->weight == right->weight )
            return left->num <= right->num;

        return left->weight > right->weight;
    }
} /* butterfly */
