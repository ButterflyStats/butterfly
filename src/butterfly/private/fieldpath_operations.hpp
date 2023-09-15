/**
 * @file fieldpath_operations.hpp
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

#ifndef BUTTERFLY_FIELDPATH_OPERATIONS_HPP
#define BUTTERFLY_FIELDPATH_OPERATIONS_HPP

#include <cstdint>
#include <vector>

#include <butterfly/util_platform.hpp>
#include <butterfly/util_bitstream.hpp>

#include "fieldpath.hpp"

namespace butterfly {
    /* clang-format off */
    void force_inline fp_PlusOne(bitstream &b, fieldpath &f) {
        f.data.back() += 1;
    }

    void force_inline fp_PlusTwo(bitstream &b, fieldpath &f) {
        f.data.back() += 2;
    }

    void force_inline fp_PlusThree(bitstream &b, fieldpath &f) {
        f.data.back() += 3;
    }

    void force_inline fp_PlusFour(bitstream &b, fieldpath &f) {
        f.data.back() += 4;
    }

    void force_inline fp_PlusN(bitstream &b, fieldpath &f) {
        f.data.back() += b.readFPBitVar() + 5;
    }

    void force_inline fp_PushOneLeftDeltaZeroRightZero(bitstream &b, fieldpath &f) {
        f.data.push_back(0);
    }

    void force_inline fp_PushOneLeftDeltaZeroRightNonZero(bitstream &b, fieldpath &f) {
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushOneLeftDeltaOneRightZero(bitstream &b, fieldpath &f){
        f.data.back() += 1;
        f.data.push_back(0);
    }

    void force_inline fp_PushOneLeftDeltaOneRightNonZero(bitstream &b, fieldpath &f){
        f.data.back() += 1;
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushOneLeftDeltaNRightZero(bitstream &b, fieldpath &f){
        f.data.back() += b.readFPBitVar();
        f.data.push_back(0);
    }

    void force_inline fp_PushOneLeftDeltaNRightNonZero(bitstream &b, fieldpath &f) {
        f.data.back() += b.readFPBitVar() + 2;
        f.data.push_back(b.readFPBitVar() + 1);
    }

    void force_inline fp_PushOneLeftDeltaNRightNonZeroPack6Bits(bitstream &b, fieldpath &f) {
        f.data.back() += b.read(3) + 2;
        f.data.push_back(b.read(3) + 1);
    }

    void force_inline fp_PushOneLeftDeltaNRightNonZeroPack8Bits(bitstream &b, fieldpath &f) {
        f.data.back() += b.read(4) + 2;
        f.data.push_back(b.read(4) + 1);
    }

    void force_inline fp_PushTwoLeftDeltaZero(bitstream &b, fieldpath &f) {
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushTwoLeftDeltaOne(bitstream &b, fieldpath &f) {
        f.data.back() += 1;
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushTwoLeftDeltaN(bitstream &b, fieldpath &f) {
        f.data.back() += b.readUBitVar() + 2;
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushTwoPack5LeftDeltaZero(bitstream &b, fieldpath &f) {
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
    }

    void force_inline fp_PushTwoPack5LeftDeltaOne(bitstream &b, fieldpath &f) {
        f.data.back() += 1;
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
    }

    void force_inline fp_PushTwoPack5LeftDeltaN(bitstream &b, fieldpath &f) {
        f.data.back() += b.readUBitVar() + 2;
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
    }

    void force_inline fp_PushThreeLeftDeltaZero(bitstream &b, fieldpath &f) {
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushThreeLeftDeltaOne(bitstream &b, fieldpath &f) {
        f.data.back() += 1;
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushThreeLeftDeltaN(bitstream &b, fieldpath &f) {
        f.data.back() += b.readUBitVar() + 2;
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
        f.data.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushThreePack5LeftDeltaZero(bitstream &b, fieldpath &f) {
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
    }

    void force_inline fp_PushThreePack5LeftDeltaOne(bitstream &b, fieldpath &f) {
        f.data.back() += 1;
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
    }

    void force_inline fp_PushThreePack5LeftDeltaN(bitstream &b, fieldpath &f) {
        f.data.back() += b.readUBitVar() + 2;
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
        f.data.push_back(b.read(5));
    }

    void force_inline fp_PushN(bitstream &b, fieldpath &f) {
        uint32_t n = b.readUBitVar();
        f.data.back() += b.readUBitVar();

        for (uint32_t i = 0; i < n; ++i) {
            f.data.push_back(b.readFPBitVar());
        }
    }

    void force_inline fp_PushNAndNonTopological(bitstream &b, fieldpath &f) {
        for (auto &idx : f.data) {
            if (b.read(1)) idx += b.readVarSInt32() + 1;
        }

        uint32_t n = b.readUBitVar();
        for (uint32_t i = 0; i < n; ++i) {
            f.data.push_back(b.readFPBitVar());
        }
    }

    void force_inline fp_PopOnePlusOne(bitstream &b, fieldpath &f) {
        f.data.pop_back();
        f.data.back() += 1;
    }

    void force_inline fp_PopOnePlusN(bitstream &b, fieldpath &f) {
        f.data.pop_back();
        f.data.back() += b.readFPBitVar() + 1;
    }

    void force_inline fp_PopAllButOnePlusOne(bitstream &b, fieldpath &f) {
        f.data.resize(1);
        f.data.back() += 1;
    }

    void force_inline fp_PopAllButOnePlusN(bitstream &b, fieldpath &f) {
        f.data.resize(1);
        f.data.back() += b.readFPBitVar() + 1;
    }

    void force_inline fp_PopAllButOnePlusNPack3Bits(bitstream &b, fieldpath &f) {
        f.data.resize(1);
        f.data.back() += b.read(3) + 1;
    }

    void force_inline fp_PopAllButOnePlusNPack6Bits(bitstream &b, fieldpath &f) {
        f.data.resize(1);
        f.data.back() += b.read(6) + 1;
    }

    void force_inline fp_PopNPlusOne(bitstream &b, fieldpath &f) {
        uint32_t nsize = f.data.size() - b.readFPBitVar();
        ASSERT_TRUE(nsize < 7 && nsize > 0,  "Invalid fp size for op");

        f.data.resize(nsize);
        f.data.back() += 1;
    }

    void force_inline fp_PopNPlusN(bitstream &b, fieldpath &f) {
        uint32_t nsize = f.data.size() - b.readFPBitVar();
        ASSERT_TRUE(nsize < 7 && nsize > 0,  "Invalid fp size for op");

        f.data.resize(nsize);
        f.data.back() += b.readVarSInt32();
    }

    void force_inline fp_PopNAndNonTopographical(bitstream &b, fieldpath &f) {
        uint32_t nsize = f.data.size() - b.readFPBitVar();
        ASSERT_TRUE(nsize < 7 && nsize > 0,  "Invalid fp size for op");

        f.data.resize(nsize);

        for (auto &idx : f.data) {
            if (b.read(1)) idx += b.readVarSInt32();
        }
    }

    void force_inline fp_NonTopoComplex(bitstream &b, fieldpath &f) {
        for (auto &idx : f.data) {
            if (b.read(1)) idx += b.readVarSInt32();
        }
    }

    void force_inline fp_NonTopoPenultimatePlusOne(bitstream &b, fieldpath &f) {
        ASSERT_TRUE(f.data.size() >= 2, "Invalid fp size for op");
        f.data[f.data.size() - 2] += 1;
    }

    void force_inline fp_NonTopoComplexPack4Bits(bitstream &b, fieldpath &f) {
        for (auto &idx : f.data) {
            if (b.read(1)) idx += b.read(4) - 7;
        }
    }

    void force_inline fp_FieldPathEncodeFinish(bitstream &b, fieldpath &f) {
        f.finished = true;
    }
    /* clang-format on */
} /* butterfly */

#endif /* BUTTERFLY_FIELDPATH_OPERATIONS_HPP */
