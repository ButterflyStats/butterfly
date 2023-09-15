/**
 * @file entity.cpp
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

#include <vector>
#include <cstdint>
#include <iostream>

#include <butterfly/entity.hpp>
#include <butterfly/flattened_serializer.hpp>
#include <butterfly/property.hpp>
#include <butterfly/util_bitstream.hpp>
#include <butterfly/util_chash.hpp>

#include "alloc.hpp"
#include "config_internal.hpp"
#include "fieldpath.hpp"
#include "fieldpath_huffman.hpp"
#include "fieldpath_operations.hpp"
#include "util_mempool.hpp"
#include "util_ascii_table.hpp"

namespace butterfly {
    entity::entity() {
        this->properties.reserve(512);
    }

    entity::~entity() {
        for ( auto& prop : properties ) {
            if ( !prop.second )
                continue;

            g_propalloc.free( prop.second );
            prop.second = nullptr;
        }

        properties.clear();
    }

    entity::entity(const entity& e) {
        this->id = e.id;
        this->cls = e.cls;
        this->type = e.type;
        this->cls_hash = e.cls_hash;

        for (auto &prop : e.properties) {
            this->properties[prop.first] = g_propalloc.malloc(*prop.second);
        }
   }

    void entity::set_serializer( const fs* serializer ) {
        this->ser = serializer;
    }

    void entity::parse( bitstream& b ) {
#if BUTTERFLY_THREADSAFE
        std::lock_guard<std::mutex> lock( mut );
#endif /* BUTTERFLY_THREADSAFE */

        static fieldpath fp;
        static std::vector<const fs*> props( 1024 );

        props.clear();
        fp.reset();

        while ( true ) {
            // Read op
            bool op_found = false;
            bool finished = false;

            for ( uint32_t i = 0, id = 0; !op_found && i < 17; ++i ) {
                id = ( id << 1 ) | b.readBool();
                fieldop_lookup( id, b, fp, op_found, finished );
            }

            ASSERT_TRUE( op_found, "Exhausted max operation bits" );

            if ( finished )
                break;

            auto& d = fp.data;

            /* clang-format off */
            switch (d.size()) {
                case 1: props.push_back(&(*ser)[d[0]]); break;
                case 2: props.push_back(&(*ser)[d[0]][d[1]]); break;
                case 3: props.push_back(&(*ser)[d[0]][d[1]][d[2]]); break;
                case 4: props.push_back(&(*ser)[d[0]][d[1]][d[2]][d[3]]); break;
                case 5: props.push_back(&(*ser)[d[0]][d[1]][d[2]][d[3]][d[4]]); break;
                case 6: props.push_back(&(*ser)[d[0]][d[1]][d[2]][d[3]][d[4]][d[5]]); break;
                default: ASSERT_TRUE( 0 != 0, "Invalid fieldpath depth");
            }
            /* clang-format on */
        }

        for ( auto& prop : props ) {
            const uint64_t fid = prop->hash;

            auto it = properties.find( fid );
            if ( it != properties.end() ) {
                prop->decoder( b, prop->info, it->second );

                #if BUTTERFLY_DEVCHECKS
                // Any hash collision means type access might be scewed
                ASSERT_TRUE( it->second->info == prop, "Hash collision in property set" );
                #endif /* BUTTERFLY_DEVCHECKS */
            } else {
                property* p = g_propalloc.malloc();
                p->info     = prop;
                prop->decoder( b, prop->info, p );
                this->properties[fid] = p;
            }
        }
    }

    void entity::spew(std::ostream &out) {
        ascii_table tbl;
        tbl.append( "Key", "Hash", "Value" );

        for ( auto& p : properties ) {
            tbl.append( p.second->info->name, p.second->info->hash, p.second->as_string() );
        }

        tbl.print( {1, 1, 1}, out );
    }
} /* butterfly */
