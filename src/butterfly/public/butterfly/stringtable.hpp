/**
 * @file stringtable.hpp
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

#ifndef BUTTERFLY_STRINGTABLE_HPP
#define BUTTERFLY_STRINGTABLE_HPP

#include <string>
#include <unordered_map>

#include <butterfly/proto/demo.pb.h>
#include <butterfly/proto/netmessages.pb.h>
#include <butterfly/util_assert.hpp>
#include <butterfly/util_dict.hpp>
#include <butterfly/util_noncopyable.hpp>

namespace butterfly {
    /** Networked stringtable containing a set of keys and values. */
    class stringtable {
    public:
        /** Type of multiindex container */
        typedef dict<std::string> container;
        /** Size type of said container */
        typedef container::size_type size_type;
        /** Value type */
        typedef container::value_type value_type;
        /** Type for an iterator */
        typedef container::iterator iterator;
        /** Type for a const iterator */
        typedef container::const_iterator const_iterator;

        /** Creates an empty stringtable, just for hashables */
        stringtable() noexcept {}

        /** Constructor, initializes table from protobuf object */
        stringtable( CSVCMsg_CreateStringTable* table );

        /** Move constructor */
        stringtable( stringtable&& o ) noexcept { swap( o ); }

        /** Move assignment operator */
        stringtable& operator=( stringtable&& o ) {
            swap( o );
            return *this;
        }

        /** Copy constructor */
        stringtable( const stringtable& o ) noexcept {
            tblName = o.tblName;
            userDataFixed = o.userDataFixed;
            userDataSize = o.userDataSize;
            userDataSizeBits = o.userDataSizeBits;
            flags = o.flags;
            table = o.table;
        }

        /** Default destructor */
        ~stringtable() = default;

        /** Swap this stringtable with given one */
        void swap( stringtable& s ) {
            std::swap( tblName, s.tblName );
            std::swap( userDataFixed, s.userDataFixed );
            std::swap( userDataSize, s.userDataSize );
            std::swap( userDataSizeBits, s.userDataSizeBits );
            std::swap( flags, s.flags );
            std::swap( table, s.table );
        }

        /** Returns iterator pointed at the beginning of the stringtable entries */
        iterator begin() { return table.begin(); }

        /**  Returns iterator pointed at the end of the stringtable entries */
        iterator end() { return table.end(); }

        /** Return number of elements stored in the stringtable */
        size_type size() { return table.size(); }

        /** Returns true if key exists */
        bool has_key( const std::string& key ) { return table.has_key( key ); }

        /** Return entry by key */
        const value_type& by_key( const std::string& key ) {
            ASSERT_TRUE( table.has_key( key ), "Trying to fetch unkown key" );
            return table.by_key( key );
        }

        /** Retruns true if index exists */
        bool has_index( uint32_t idx ) { return table.has_index( idx ); }

        /** Return entry by index */
        const value_type& by_index( uint32_t idx ) {
            ASSERT_TRUE( table.has_index( idx ), "Trying to fetch unkown key" );
            return table.by_index( idx );
        }

        /** Remove all entries */
        void clear() { table.clear(); }

        /** Returns name of this stringtable */
        const std::string& name() const { return tblName; }

        /** Update table from protobuf */
        void update( CSVCMsg_UpdateStringTable* table );

        /** Update from full packet */
        void update( const CDemoStringTables_table_t& items );

    private:
        /** Name of this stringtable */
        std::string tblName;
        /** Whether the data read from updates has a fixed size */
        bool userDataFixed;
        /** Size of data in bytes */
        uint32_t userDataSize;
        /** Size of data in bits */
        uint32_t userDataSizeBits;
        /** Flags for this table */
        int32_t flags;
        /** List of stringtable entries */
        container table;

        /** Update table from raw data */
        void update( const uint32_t& entries, const std::string& data );
    };
} /* butterfly */

#endif /* BUTTERFLY_STRINGTABLE_HPP */
