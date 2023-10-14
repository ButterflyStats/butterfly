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
        f.back() += 1;
    }

    void force_inline fp_PlusTwo(bitstream &b, fieldpath &f) {
        f.back() += 2;
    }

    void force_inline fp_PlusThree(bitstream &b, fieldpath &f) {
        f.back() += 3;
    }

    void force_inline fp_PlusFour(bitstream &b, fieldpath &f) {
        f.back() += 4;
    }

    void force_inline fp_PlusN(bitstream &b, fieldpath &f) {
        f.back() += b.readFPBitVar() + 5;
    }

    void force_inline fp_PushOneLeftDeltaZeroRightZero(bitstream &b, fieldpath &f) {
        f.push_back(0);
    }

    void force_inline fp_PushOneLeftDeltaZeroRightNonZero(bitstream &b, fieldpath &f) {
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushOneLeftDeltaOneRightZero(bitstream &b, fieldpath &f){
        f.back() += 1;
        f.push_back(0);
    }

    void force_inline fp_PushOneLeftDeltaOneRightNonZero(bitstream &b, fieldpath &f){
        f.back() += 1;
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushOneLeftDeltaNRightZero(bitstream &b, fieldpath &f){
        f.back() += b.readFPBitVar();
        f.push_back(0);
    }

    void force_inline fp_PushOneLeftDeltaNRightNonZero(bitstream &b, fieldpath &f) {
        f.back() += b.readFPBitVar() + 2;
        f.push_back(b.readFPBitVar() + 1);
    }

    void force_inline fp_PushOneLeftDeltaNRightNonZeroPack6Bits(bitstream &b, fieldpath &f) {
        f.back() += b.read(3) + 2;
        f.push_back(b.read(3) + 1);
    }

    void force_inline fp_PushOneLeftDeltaNRightNonZeroPack8Bits(bitstream &b, fieldpath &f) {
        f.back() += b.read(4) + 2;
        f.push_back(b.read(4) + 1);
    }

    void force_inline fp_PushTwoLeftDeltaZero(bitstream &b, fieldpath &f) {
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushTwoLeftDeltaOne(bitstream &b, fieldpath &f) {
        f.back() += 1;
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushTwoLeftDeltaN(bitstream &b, fieldpath &f) {
        f.back() += b.readUBitVar() + 2;
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushTwoPack5LeftDeltaZero(bitstream &b, fieldpath &f) {
        f.push_back(b.read(5));
        f.push_back(b.read(5));
    }

    void force_inline fp_PushTwoPack5LeftDeltaOne(bitstream &b, fieldpath &f) {
        f.back() += 1;
        f.push_back(b.read(5));
        f.push_back(b.read(5));
    }

    void force_inline fp_PushTwoPack5LeftDeltaN(bitstream &b, fieldpath &f) {
        f.back() += b.readUBitVar() + 2;
        f.push_back(b.read(5));
        f.push_back(b.read(5));
    }

    void force_inline fp_PushThreeLeftDeltaZero(bitstream &b, fieldpath &f) {
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushThreeLeftDeltaOne(bitstream &b, fieldpath &f) {
        f.back() += 1;
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushThreeLeftDeltaN(bitstream &b, fieldpath &f) {
        f.back() += b.readUBitVar() + 2;
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
        f.push_back(b.readFPBitVar());
    }

    void force_inline fp_PushThreePack5LeftDeltaZero(bitstream &b, fieldpath &f) {
        f.push_back(b.read(5));
        f.push_back(b.read(5));
        f.push_back(b.read(5));
    }

    void force_inline fp_PushThreePack5LeftDeltaOne(bitstream &b, fieldpath &f) {
        f.back() += 1;
        fp_PushThreePack5LeftDeltaZero(b, f);
    }

    void force_inline fp_PushThreePack5LeftDeltaN(bitstream &b, fieldpath &f) {
        f.back() += b.readUBitVar() + 2;
        fp_PushThreePack5LeftDeltaZero(b, f);
    }

    void force_inline fp_PushN(bitstream &b, fieldpath &f) {
        uint32_t n = b.readUBitVar();
        f.back() += b.readUBitVar();

        for (uint32_t i = 0; i < n; ++i) {
            f.push_back(b.readFPBitVar());
        }
    }

    void force_inline fp_PushNAndNonTopological(bitstream &b, fieldpath &f) {
        for (auto &idx : f) {
            if (b.readBool()) idx += b.readVarSInt32() + 1;
        }

        uint32_t n = b.readUBitVar();
        for (uint32_t i = 0; i < n; ++i) {
            f.push_back(b.readFPBitVar());
        }
    }

    void force_inline fp_PopOnePlusOne(bitstream &b, fieldpath &f) {
        f.pop_back();
        f.back() += 1;
    }

    void force_inline fp_PopOnePlusN(bitstream &b, fieldpath &f) {
        f.pop_back();
        f.back() += b.readFPBitVar() + 1;
    }

    void force_inline fp_PopAllButOnePlusOne(bitstream &b, fieldpath &f) {
        f.resize(1);
        f.back() += 1;
    }

    void force_inline fp_PopAllButOnePlusN(bitstream &b, fieldpath &f) {
        f.resize(1);
        f.back() += b.readFPBitVar() + 1;
    }

    void force_inline fp_PopAllButOnePlusNPack3Bits(bitstream &b, fieldpath &f) {
        f.resize(1);
        f.back() += b.read(3) + 1;
    }

    void force_inline fp_PopAllButOnePlusNPack6Bits(bitstream &b, fieldpath &f) {
        f.resize(1);
        f.back() += b.read(6) + 1;
    }

    void force_inline ApplyPopN(bitstream &b, fieldpath &f) {
        int32_t n = b.readFPBitVar();
        ASSERT_TRUE(f.size() >= n, "Invalid fp size for op");

        f.resize(std::max(f.size(), 1 + static_cast<size_t>(n)) - n);
    }
    void force_inline fp_PopNPlusOne(bitstream &b, fieldpath &f) {
        ApplyPopN(b, f);
        f.back() += 1;
    }

    void force_inline fp_PopNPlusN(bitstream &b, fieldpath &f) {
        ApplyPopN(b, f);
        f.back() += b.readVarSInt32();
    }

    void force_inline fp_NonTopoComplex(bitstream &b, fieldpath &f) {
        for (auto &idx : f) {
            if (b.readBool()) idx += b.readVarSInt32();
        }
    }
    void force_inline fp_PopNAndNonTopological(bitstream &b, fieldpath &f) {
        ApplyPopN(b, f);
        fp_NonTopoComplex(b, f);
    }

    void force_inline fp_NonTopoPenultimatePlusOne(bitstream &b, fieldpath &f) {
        ASSERT_TRUE(f.size() >= 2, "Invalid fp size for op");
        f[f.size() - 2] += 1;
    }

    void force_inline fp_NonTopoComplexPack4Bits(bitstream &b, fieldpath &f) {
        for (auto &idx : f) {
            if (b.readBool())
                idx += static_cast<int16_t>(b.read(4)) - 7; // signed 4 bits addition by using hack
        }
    }
    /* clang-format on */
} /* butterfly */

#endif /* BUTTERFLY_FIELDPATH_OPERATIONS_HPP */
