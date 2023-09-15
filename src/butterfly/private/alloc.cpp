/**
 * @file alloc.cpp
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

#include <butterfly/flattened_serializer.hpp>
#include <butterfly/entity.hpp>

#include "alloc.hpp"

namespace butterfly {
    object_pool<fs_typeinfo> g_fstypeinfoalloc( 4096 );
    object_pool<fs_info> g_fsinfoalloc( 4096 );
    object_pool<fs_table_data> g_fstabledataalloc( 4096 );
    object_pool<fs> g_fsalloc( 64 * 1024 );
    object_pool<entity> g_entalloc( 2048 );
}
