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
#include <array>

namespace butterfly {
    /** Type for a source 2 fieldpath */
    class fieldpath {
    public:
        force_inline fieldpath() {}
        force_inline fieldpath(size_t size) : size_((uint8_t)size) {}

        force_inline size_t size() const {
            return this->size_;
        }
        force_inline bool empty() const {
            return this->size_ == 0;
        }

        force_inline uint16_t& operator[](uint32_t i) {
            return this->ar[i];
        }

        force_inline uint16_t operator[](uint32_t i) const {
            return this->ar[i];
        }

        force_inline bool operator==(const fieldpath& other) const {
            if (this->size_ != other.size_)
                return false;
            return std::equal(this->ar.begin(), this->ar.begin() + this->size_, other.ar.begin());
        }

        force_inline void push_back(uint16_t val) {
            ASSERT_LESS(this->size_, this->ar.size(), "No space for new fieldpath element");
            this->ar[this->size_++] = val;
        }

        force_inline void resize(size_t size) {
            ASSERT_LESS_EQ(size, this->ar.size(), "No space for fieldpath resize");
            this->size_ = size;
        }

        force_inline uint16_t& back() {
            ASSERT_GREATER(this->size_, 0, "No elements available");
            return this->ar[this->size_ - 1];
        }

        force_inline const uint16_t* data() const {
            return this->ar.data();
        }

        force_inline void pop_back() {
            ASSERT_GREATER(this->size_, 0, "No elements available");
            this->size_--;
        }

        force_inline auto begin() {
            return this->ar.begin();
        }

        force_inline auto end() {
            return this->ar.begin() + this->size();
        }

        /** Dump fieldpath */
        void spew() {
            /* clang-format off */
            switch ( size() ) {
                case 1: printf("%d\n", ar[0]); break;
                case 2: printf("%d/%d\n", ar[0], ar[1]); break;
                case 3: printf("%d/%d/%d\n", ar[0], ar[1], ar[2]); break;
                case 4: printf("%d/%d/%d/%d\n", ar[0], ar[1], ar[2], ar[3]); break;
                case 5: printf("%d/%d/%d/%d/%d\n", ar[0], ar[1], ar[2], ar[3], ar[4]); break;
                case 6: printf("%d/%d/%d/%d/%d/%d\n", ar[0], ar[1], ar[2], ar[3], ar[4], ar[5]); break;
                case 7: printf("%d/%d/%d/%d/%d/%d/%d\n", ar[0], ar[1], ar[2], ar[3], ar[4], ar[5], ar[6]); break;
                default: printf("Invalid Fieldpath\n");
            }
            /* clang-format on */
        }

    private:
        uint8_t size_ = 0;
        std::array<uint16_t, 7> ar{};
    };
} /* butterfly */

#endif /* BUTTERFLY_FIELDPATH_HPP */
