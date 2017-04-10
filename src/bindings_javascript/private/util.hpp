/**
 * @file util.hpp
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
 *
 * @par Description
 *    Utilities to use alongside the JS api
 */

#ifndef BUTTERFLYJS_UTIL_HPP
#define BUTTERFLYJS_UTIL_HPP

#include <emscripten.h>
#include <emscripten/bind.h>

#include <butterfly/entity.hpp>
#include <butterfly/util_dict.hpp>

namespace butterfly {
    /** Gets all keys in a map */
    template <typename T, typename V>
    std::vector<T> get_map_keys(std::unordered_map<T, V>& map) {
        std::vector<T> ret;

        for (auto &entry : map) {
            ret.push_back(entry.first);
        }

        return ret;
    }

    /** Get all keys / indices in a dict */
    template <typename T>
    std::vector<std::size_t> get_dict_keys(dict<T>& dict) {
        std::vector<std::size_t> ret;

        for (auto &entry : dict) {
            ret.push_back(entry.index);
        }

        return ret;
    }

    /** Converts a dict to an emscripten JS value */
    template <typename T>
    val dict_to_em(dict<T>& dict) {
        val ret = val::object();

        for (auto &e : dict) {
            val ele = val::object();
            ele.set("index", e.index);
            ele.set("key", e.key);
            ele.set("value", e.value);

            ret.set(e.key, ele);
        }

        return ret;
    }

    /**
     * Create a binding for a dictionary
     *
     * Dict bindings are very different from python or the original C++ code.
     * We know that we currently don't have any dictionaries that hold pointers,
     * hence why we can convert the value to a pointer.
     * We do this because we'd have to call .delete() on every chained element
     * otherwise, unless we are fine with going OOM very quick due to leakage.
     */
    template <typename T>
    void bind_dict(std::string const &name) {
        // Dictionary
        using Dict = dict<T>;

        // Entry
        using DictEntry = typename dict<T>::entry_t;
        std::string ename = name+"Entry";

        class_<DictEntry>(ename.c_str())
            .property("index", &DictEntry::index)
            .property("key", &DictEntry::key)
            .function("value", select_overload<T* (DictEntry&)>(
                [](DictEntry& d) {
                    return &d.value;
                }
            ), allow_raw_pointer<arg<0>>());

        // Main
        class_<Dict>(name.c_str())
        #ifdef DEVELOPER
            .function("size", &Dict::size)
            .function("reserve", &Dict::reserve)
            .function("clear", &Dict::clear)
        #endif /* DEVELOPER */
            .function("has_key", &Dict::has_key)
            .function("has_index", &Dict::has_index)
            .function("by_key", select_overload<DictEntry* (Dict&, std::string k)>(
                [](Dict& d, std::string k) -> DictEntry* {
                    return &d.by_key(k);
                }
            ), allow_raw_pointer<arg<0>>())
            .function("by_index", select_overload<DictEntry* (Dict&, uint32_t k)>(
                [](Dict& d, uint32_t k) -> DictEntry* {
                    return &d.by_index(k);
                }
            ), allow_raw_pointer<arg<0>>())
            .function("indices", select_overload<std::vector<std::size_t> (Dict&)>(
                [](Dict& d) {
                    // very slow, but there is no better way to do this atm
                    return get_dict_keys(d);
                }
            ));
    }
}

#endif /* BUTTERFLYJS_UTIL_HPP */
