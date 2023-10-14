/**
 * @file alloc.hpp
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

#ifndef BUTTERFLY_ALLOC_HPP
#define BUTTERFLY_ALLOC_HPP

#include "util_mempool.hpp"

namespace butterfly {
    // forward decl
    struct fs_typeinfo;
    struct fs_info;
    struct fs_table_data;
    struct fs;
    class entity;

    /// Flattened serializer type info allocator (global)
    extern object_pool<fs_typeinfo> g_fstypeinfoalloc;

    /// Flattened serializer info allocator (global)
    extern object_pool<fs_info> g_fsinfoalloc;

    /// Flattened serializer table data allocator (global)
    extern object_pool<fs_table_data> g_fstabledataalloc;

    /// Flattened serializer allocator (global)
    extern object_pool<fs> g_fsalloc;

    /// Entity allocator (global)
    extern object_pool<entity> g_entalloc;
} /* butterfly */

#endif /* BUTTERFLY_ALLOC_HPP */
