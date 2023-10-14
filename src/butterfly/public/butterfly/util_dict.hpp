/**
 * @file util_dict.hpp
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

#ifndef BUTTERFLY_UTIL_DICT_HPP
#define BUTTERFLY_UTIL_DICT_HPP

#include <string>
#include <vector>
#include <type_traits>
#include <unordered_map>
#include <butterfly/util_assert.hpp>

namespace butterfly {
    namespace detail {
        template <typename T>
        struct ref_or_pointer {
            static T* apply( T& t ) { return &t; }
        };

        template <typename T>
        struct ref_or_pointer<T*> {
            static T* apply( T* t ) { return t; }
        };
    }

    /** Dictionary aka multi index map */
    template <typename Value>
    class dict {
    public:
        /** Structure for a single entry */
        struct entry_t {
            /** Numeric index of entry */
            std::size_t index;
            /** Key of entry */
            std::string key;
            /** Value */
            Value value;

            /** Member access via operator overload */
            template <typename T = Value>
            auto operator-> () -> decltype( detail::ref_or_pointer<T>::apply( value ) ) {
                return detail::ref_or_pointer<T>::apply( value );
            }
        };

    private:
        typedef std::unordered_map<std::string, std::size_t> map_t;
        typedef std::vector<entry_t> vector_t;

    public:
        /** Value type */
        typedef entry_t value_type;
        /** Size type */
        typedef typename vector_t::size_type size_type;
        /** Iterator */
        typedef typename vector_t::iterator iterator;
        /** Const iterattor */
        typedef typename vector_t::const_iterator const_iterator;

        /** Returns iterator to beginning of entry list */
        iterator begin() { return entries.begin(); }

        /** Returns iterator to end of entry list */
        iterator end() { return entries.end(); }

        /** Returns const pointer to begining of entry list */
        const_iterator cbegin() { return entries.cbegin(); }

        /** Returns const pointer to end of entry list */
        const_iterator cend() { return entries.cend(); }

        /** Returns size of list */
        size_t size() const { return entries.size(); }

        /** Reserves memory for a certain number of entries */
        void reserve( const size_type size ) { entries.reserve( size ); }

        /** Remove all entries from the dictionary */
        void clear() {
            keyMap.clear();
            entries.clear();
        }

        /** Inserts a new entry */
        template <typename V,
            typename = std::enable_if<std::is_same<Value, typename std::remove_reference<V>::type>::value>>
        value_type& insert( std::size_t index, std::string key, V&& v ) {
            if ( entries.size() <= index )
                entries.resize( index + 1 );

            ASSERT_TRUE( keyMap.find( key ) == keyMap.end(), "Entry key collision" );

            entries[index] = entry_t{index, key, std::forward<V>( v )};
            keyMap[key]    = index;

            return entries[index];
        }

        /** Checks if specified key exists */
        bool has_key( const std::string& key ) const { return ( keyMap.find( key ) != keyMap.end() ); }

        /** Checks if index is valid */
        bool has_index( std::size_t idx ) const { return entries.size() > idx; }

        /** Returns entry by key */
        entry_t& by_key( const std::string& key ) { return entries[keyMap[key]]; }
        const entry_t* find_by_key( const std::string& key ) const {
            auto id = keyMap.find( key );
            if ( id == keyMap.end() )
                return nullptr;
            return &entries[id->second];
        }

        /** Returns entry by index */
        entry_t& by_index( std::size_t idx ) {
            ASSERT_LESS( idx, entries.size(), "Invalid entry index" );
            return entries[idx];
        }
        const entry_t& by_index( std::size_t idx ) const {
            ASSERT_LESS( idx, entries.size(), "Invalid entry index" );
            return entries[idx];
        }

    private:
        /** Key -> Index Mapping */
        std::unordered_map<std::string, std::size_t> keyMap;
        /** Index -> Entry Mapping */
        std::vector<entry_t> entries;
    };
} /* butterfly */

#endif /* BUTTERFLY_UTIL_DICT_HPP */
