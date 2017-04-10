/**
 * @file resources.cpp
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

#include <mutex>
#include <string>

#include <butterfly/trie/tx.hpp>
#include <butterfly/resources.hpp>
#include "config_internal.hpp"

// trie data
unsigned char trie_data[] = {
#include "resources.trie.inline"
};

// Hash entry creation macro
#define ENTRY( _HASH, _TID )                                                                                           \
    case _HASH: {                                                                                                      \
        trie.reverseLookup( _TID, ret );                                                                               \
        return ret;                                                                                                    \
    }

// Ressource lock
static std::mutex g_rlock;

// trie
static tx_tool::tx trie;
static bool trie_init = false;

namespace butterfly {
    /** Lookup resources hash and return corresponding entry */
    std::string resource_lookup( uint64_t hash ) {
#if BUTTERFLY_THREADSAFE
        // look this in thread safe mode
        std::lock_guard<std::mutex> lock( mut );
#endif /* BUTTERFLY_THREADSAFE */

        if ( !trie_init ) {
            trie_init = true;
            trie.read( (const char*)trie_data, sizeof( trie_data ) );
        }

        std::string ret;

        switch ( hash ) {
#include "resources.hash.inline"
        default:
            return "unknown";
        }
    }
}
