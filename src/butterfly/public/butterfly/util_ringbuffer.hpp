/**
 * @file util_ringbuffer.hpp
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

#ifndef BUTTERFLY_UTIL_RINGBUFFER_HPP
#define BUTTERFLY_UTIL_RINGBUFFER_HPP

#include <vector>
#include <cstdint>
#include <limits.h>

#include <butterfly/util_assert.hpp>

namespace butterfly {
    /** Circular buffer that overwrites previously written data after maximum size has been reached */
    template <typename T>
    class ringbuffer {
    public:
        /** Constructor */
        ringbuffer( uint32_t capacity = 128 ) : capacity( capacity ), data( capacity ), pos( 0 ), wraparound( false ) {}

        /** Destructor */
        ~ringbuffer() = default;

        /** Copy constructor */
        ringbuffer( const ringbuffer& r )
            : capacity( r.capacity ), data( r.data ), pos( r.pos ), wraparound( r.wraparound ) {}

        /** Move constructor */
        ringbuffer( ringbuffer&& r )
            : capacity( r.capacity ), data( std::move( r.data ) ), pos( r.pos ), wraparound( r.wraparound ) {
            r.data.clear();
            r.pos        = 0;
            r.capacity   = 0;
            r.wraparound = false;
        }

        /** Copy assignment operator */
        ringbuffer& operator=( const ringbuffer& r ) {
            std::swap( data, r.data );
            std::swap( pos, r.pos );
            std::swap( capacity, r.capacity );
            std::swap( wraparound, r.wraparound );

            return this;
        }

        /** Move assignment operator */
        ringbuffer& operator=( ringbuffer&& r ) {
            data       = std::move( r.data );
            pos        = r.pos;
            capacity   = r.capacity;
            wraparound = r.wraparound;

            r.pos        = 0;
            r.size       = 0;
            r.wraparound = false;

            return this;
        }

        /** Returns the current position */
        uint32_t position() { return pos % capacity; }

        /** Returns maximum number of elements */
        uint32_t max() { return capacity; }

        /** Returns size */
        uint32_t size() {
            if ( pos > capacity || wraparound ) {
                return capacity;
            } else {
                return pos;
            }
        }

        /** Resize, can only grow */
        void resize( uint32_t new_capacity ) {
            std::vector<T> data_new( new_capacity );

            for ( uint32_t i = 0; ( i < capacity ) && ( i < pos ); ++i ) {
                data_new[i] = data[index_for( i )];
            }

            data = data_new;

            if ( pos > capacity ) {
                pos        = capacity;
                wraparound = false;
            }

            capacity = new_capacity;
        }

        /** Insert a new entry */
        template <typename V>
        void insert( V&& ele ) {
            if ( pos == UINT_MAX )
                wraparound = true;

            uint32_t idx = pos++ % capacity;
            data[idx]    = std::forward<T>( ele );
        }

        /** Return entry at given position */
        T& at( uint32_t idx ) { return data.at( index_for( idx ) ); }

        /** Returns reference to underlying data */
        std::vector<T>& get_data() { return data; }

    private:
        /** Maximum number of elements in the buffer */
        uint32_t capacity;
        /** Data */
        std::vector<T> data;
        /** Current position */
        uint32_t pos;
        /** Did we wrap around? */
        bool wraparound;

        uint32_t index_for( uint32_t idx ) {
            ASSERT_TRUE( idx < capacity, "Out-of-bounds request" );

            // if we haven't wrapped around yet, always return 0
            if ( pos < capacity )
                return idx;

            return ( pos + idx ) % capacity;
        }
    };
}

#endif /* BUTTERFLY_UTIL_RINGBUFFER_HPP */
