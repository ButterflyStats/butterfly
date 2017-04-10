/**
 * @file config_internal.hpp
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

#ifndef BUTTERFLY_CONFIG_INTERNAL_HPP
#define BUTTERFLY_CONFIG_INTERNAL_HPP

/// Source 2 verification header
#define BUTTERFLY_S2_HEADER "PBDEMS2"

/// Number of bytes to allocate for decompression
#define BUTTERFLY_SNAPPY_BUFFER_SIZE 204800

/// Ifstream chunk size
#define BUTTERFLY_IFSTREAM_CHUNK_SIZE 2048000

/// Enable / disable mempool
#define BUTTERFLY_OBJECTPOOL_DISABLE 0

/// Maximum objects to allocate per page
#define BUTTERFLY_OBJECTPOOL_PAGESIZE 2048

/// Factor to grow page size by
#define BUTTERFLY_OBJECTPOOL_GROW 2

/// Thread-Safe?
#define BUTTERFLY_THREADSAFE 0

/// Developer checks
#define BUTTERFLY_DEVCHECKS 0

/// Maximum number of entities
#define BUTTERFLY_MAX_ENTS 20480

#endif /* BUTTERFLY_CONFIG_INTERNAL_HPP */
