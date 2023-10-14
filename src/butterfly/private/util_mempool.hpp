/**
 * @file util_mempool.hpp
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

#ifndef BUTTERFLY_UTIL_MEMPOOL_HPP
#define BUTTERFLY_UTIL_MEMPOOL_HPP

#include <mutex>
#include <cstdint>

#include <butterfly/util_assert.hpp>
#include <butterfly/util_noncopyable.hpp>

#include "config_internal.hpp"

namespace butterfly {
    /** Simple memory pool for allocating same-size objects */
    template <typename T>
    class object_pool : private noncopyable {
    public:
        /** Constructor, optionally takes initial capacity */
        explicit object_pool( size_t initial_capacity = 128 )
            : freelist( nullptr ), page_size( 0 ), page_first( page( initial_capacity ) ) {
            page_last     = &page_first;
            page_mem      = page_first.mem;
            page_capacity = page_first.capacity;
        }

        /** Free pool memory */
        ~object_pool() {
            page* current = page_first.next;

            while ( current ) {
                page* next = current->next;
                delete current;
                current = next;
            }

            page_mem = nullptr;
        }

        /** Allocate new T */
        template <typename... Args>
        T* malloc( Args... args ) {
#if BUTTERFLY_OBJECTPOOL_DISABLE
            return new T( args... );
#endif /* BUTTERFLY_OBJECTPOOL_DISABLE */

            T* addr = nullptr;

            {
#if BUTTERFLY_THREADSAFE
                std::lock_guard<std::mutex> lock( mut );
#endif /* BUTTERFLY_THREADSAFE */

                ASSERT_TRUE( page_mem != nullptr, "Objectpool already deallocated" );

                // Return a reserved spot
                if ( freelist ) {
                    T* res   = freelist;
                    freelist = *reinterpret_cast<T**>( freelist );
                    new ( res ) T( args... );
                    return res;
                }

                // Allocate a new node if necessary
                if ( page_size >= page_capacity ) {
                    allocate_page();
                }

                // Allocate object
                addr = &reinterpret_cast<T*>( page_mem.load() )[page_size];
                ++page_size;
            }
            return new ( addr ) T( args... );
        }

        /** Free T allocated by us */
        void free( T* obj ) {
#if BUTTERFLY_OBJECTPOOL_DISABLE
            delete obj;
            return;
#endif /* BUTTERFLY_OBJECTPOOL_DISABLE */

            if ( page_mem == nullptr )
                return;

            obj->~T();
            
#if BUTTERFLY_THREADSAFE
            std::lock_guard<std::mutex> lock( mut );
#endif /* BUTTERFLY_THREADSAFE */

            *reinterpret_cast<T**>( obj ) = freelist;
            freelist      = obj;
        }

    private:
        /** Allocate a new page */
        void allocate_page() {
            page* npage     = new page( page_size );
            page_last->next = npage;
            page_last       = npage;
            page_capacity   = page_last->capacity;
            page_mem        = page_last->mem;
            page_size       = 0;
        }

    private:
        /** Memory Page */
        struct page {
            /** Points to next page if any */
            page* next;
            /** Points to allocated memory */
            void* mem;
            /** Number of items allocated */
            size_t capacity;

            /** Allocate a new page */
            page( size_t capacity ) : next( nullptr ), mem( nullptr ), capacity( capacity ) {
                mem = ::operator new( sizeof( T ) * capacity );
            }

            /** Free page memory */
            ~page() { ::operator delete( mem ); }
        };

        /** First deleted object */
        T* freelist;
        /** Number of elements allocated for the current page */
        size_t page_size;
        /** First page */
        page page_first;
        /** Last allocated page */
        page* page_last;
        /** Pointer to page_last->mem */
        std::atomic<void*> page_mem;
        /** page_last->capacity */
        size_t page_capacity;
        /** Mutex */
        std::mutex mut;
    };
} /* butterfly */

#endif /* BUTTERFLY_UTIL_MEMPOOL_HPP */
