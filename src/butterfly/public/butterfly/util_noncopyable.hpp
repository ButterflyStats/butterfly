/**
 * @file util_noncopyable.hpp
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

#ifndef BUTTERFLY_UTIL_NONCOPYABLE_HPP
#define BUTTERFLY_UTIL_NONCOPYABLE_HPP

namespace butterfly {
    /** Inheriting from this class makes the target uncopyable */
    class noncopyable {
    protected:
        constexpr noncopyable() = default;
        ~noncopyable()          = default;

    private:
        noncopyable( const noncopyable& ) = delete;
        noncopyable& operator=( const noncopyable& ) = delete;
    };
} /* butterfly */

#endif /* BUTTERFLY_UTIL_NONCOPYABLE_HPP */
