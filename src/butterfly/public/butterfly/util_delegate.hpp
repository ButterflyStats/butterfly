/**
 * @file util_delegate.hpp
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

#ifndef BUTTERFLY_UTIL_DELEGATE_HPP
#define BUTTERFLY_UTIL_DELEGATE_HPP

namespace butterfly {
    template <typename T>
    class delegate;

    /** A very simple delegate implementation, does not respect const for now */
    template <typename R, typename ...Args>
    class delegate<R (Args...)> {
    private:
        /** Function pointer to functor */
        typedef R (*fcn_ptr_t)(void*, Args&&...);
    public:
        /** Constructor */
        delegate() : obj_ptr(nullptr), fcn_ptr(nullptr) {}

        /** Copy constructor */
        delegate(const delegate&) = default;

        /** Move constructor */
        delegate(delegate&&) = default;

        /** Destructor */
        ~delegate() = default;

        /** Create delegate from free function */
        template <R (* const fcn)(Args...)>
        static delegate from() noexcept {
            return delegate(nullptr, caller_free<fcn>);
        }

        /** Create delegate from member function */
        template <typename T, R (T::*fcn)(Args...)>
        static delegate from(T* obj) noexcept {
            return delegate(obj, caller_member<T, fcn>);
        }

        /** Call */
        R operator()(Args... args) const {
          return fcn_ptr(obj_ptr, std::forward<Args>(args)...);
        }

        /** Reset delegate state */
        void reset() {
            obj_ptr = nullptr;
            fcn_ptr = nullptr;
        }

        /** == operator */
        bool operator==(delegate const& rhs) const noexcept {
          return (rhs.obj_ptr == obj_ptr) && (rhs.fcn_ptr == fcn_ptr);
        }

        /** != operator */
        bool operator!=(delegate const& rhs) const noexcept {
          return !(operator==(rhs));
        }

        /** bool operator, check if delegate is initialized */
        explicit operator bool() const noexcept {
            return fcn_ptr != nullptr;
        }
    private:
        /** Pointer to bound object if applicable */
        void* obj_ptr;
        /** Function pointer internal forwarding function */
        fcn_ptr_t fcn_ptr;

        /** Constructor with obj / fcn ptr set */
        delegate(void* obj_ptr, fcn_ptr_t fcn_ptr) : obj_ptr(obj_ptr), fcn_ptr(fcn_ptr) {}

        /** Caller for free functions */
        template <R (*fcn)(Args...)>
        static R caller_free(void*, Args&&... args) {
            return fcn(std::forward<Args>(args)...);
        }

        /** Caller for member function pointers */
        template <typename T, R (T::*fcn)(Args...)>
        static R caller_member(void* obj, Args&&... args) {
            return (static_cast<T*>(obj)->*fcn)(std::forward<Args>(args)...);
        }
    };
}

#endif /* BUTTERFLY_UTIL_DELEGATE_HPP */
