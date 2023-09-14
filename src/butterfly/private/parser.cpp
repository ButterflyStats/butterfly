/**
 * @file parser.cpp
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

#include <string>
#include <vector>
#include <cstdint>

#include <butterfly/proto/demo.pb.h>
#include <butterfly/proto/netmessages.pb.h>
#include <butterfly/proto/gameevents.pb.h>

#include <butterfly/util_bitstream.hpp>
#include <butterfly/dem.hpp>
#include <butterfly/demfile.hpp>
#include <butterfly/entity.hpp>
#include <butterfly/entity_classes.hpp>
#include <butterfly/flattened_serializer.hpp>
#include <butterfly/packets.hpp>
#include <butterfly/parser.hpp>
#include <butterfly/property.hpp>
#include <butterfly/stringtable.hpp>
#include <butterfly/util_assert.hpp>
#include <butterfly/util_chash.hpp>
#include <butterfly/util_ztime.hpp>
#include <butterfly/visitor.hpp>

#include "alloc.hpp"
#include "util_mempool.hpp"
#include "util_varint.hpp"

#define E_CREATE 1
#define E_UPDATE 2
#define E_LEAVE 3
#define E_DELETE 4

namespace butterfly {
    parser::parser( )
        : dem( nullptr ), buildnumber( 0 ), serializers( nullptr ), packets( 2048, false ), seekPos( 0 ) {
        entities.resize( BUTTERFLY_MAX_ENTS, nullptr );
    }

    parser::~parser() {
        if ( dem )
            delete dem;

        for ( auto& e : entities ) {
            if ( e ) {
                g_entalloc.free( e );
                e = nullptr;
            }
        }

        for ( auto& e : baselines ) {
            if ( e ) {
                g_entalloc.free( e );
                e = nullptr;
            }
        }

        if ( serializers )
            delete serializers;
    }

    void parser::open( const char* path, visitor* v ) {
        if ( dem ) {
            delete dem;
            dem = nullptr;
        }

        if (v) {
            v->p = this;
            dem = new demfile( path, [=](float f){
                v->on_progress(f);
            });
        } else {
          dem = new demfile( path, nullptr );
        }

        seekPos = dem->pos();
    }

    void parser::reset() {
        // clear tables individually
        for ( auto& tbl : stringtables ) {
            tbl->clear();
        }

        stringtables.clear();

        for ( auto& e : entities ) {
            if ( e ) {
                g_entalloc.free( e );
                e = nullptr;
            }
        }

        for ( auto& e : baselines ) {
            if ( e ) {
                g_entalloc.free( e );
                e = nullptr;
            }
        }
    }

    void parser::parse( visitor* v ) {
        if ( v ) v->p = this;

        if ( v && !dem->good() )
            v->on_state( parser::END );

        dem_packet p = dem->get();

        if (v && (p.tick != tick)) {
            v->on_tick(p.tick);
        }

        tick = p.tick;

        switch ( p.type ) {
        case DEM_FileHeader:
            this->dem_handle_file_header( p );
            break;
        case DEM_SendTables:
            if ( !serializers )
                this->dem_handle_send_tables( p );
            break;
        case DEM_ClassInfo:
            if ( !classes->size() )
                this->dem_handle_class_info( p );

            if ( v )
                v->on_state( parser::SENDTABLES );
            break;
        case DEM_Packet: {
            CDemoPacket proto;
            ASSERT_TRUE( proto.ParseFromArray( p.data, p.size ), "Unable to parse protobuf packet" );

            // Can only be parsed as a bitstream
            bitstream bs( proto.data() );
            this->dem_handle_packet( bs, v );
        } break;
        case DEM_SignonPacket: {
            CDemoPacket proto;
            ASSERT_TRUE( proto.ParseFromArray( p.data, p.size ), "Unable to parse protobuf packet" );

            // Can only be parsed as a bitstream
            bitstream bs( proto.data() );
            this->dem_handle_packet( bs, v );
        } break;
        }
    }

    void parser::parse_all( visitor* v ) {
        if ( v ) {
            v->p = this;
            v->on_state( parser::BEGIN );
        }

        tick = 0;

        // Read and handle all packets
        while ( dem->good() ) {
            parse( v );
        }

        if ( v )
            v->on_state( parser::END );
    }

    void parser::require( uint32_t id ) {
        ASSERT_TRUE( id < packets.size(), "Overflow in require detected" );
        packets[id] = true;
    }

    void parser::seek( uint32_t time ) {
        ASSERT_TRUE( seekPos != 0, "Seeking is only available after on_state(SENDTABLES) has been dispatched" );

        reset(); // soft reset, entities and stringtables
        dem->set_pos( seekPos );

        bool trigger = false;

        while (dem->good()) {
            entity* e = nullptr;
            auto lpos = dem->pos();
            dem_packet p = dem->get();

            if (time <= 61) {
              trigger = true;
            }

            // handle fullpackets in stream
            if (p.type == DEM_FullPacket) {
                // parse protobuf
                CDemoFullPacket proto;
                proto.ParseFromArray( p.data, p.size );
                trigger = true;

                // apply all stringtable-create packets first
                bitstream bs( proto.packet().data() );
                while ( bs.remaining() > 8 ) {
                    alignas( 8 ) char data[1000];
                    uint32_t type = bs.readUBitVar();
                    uint32_t size = bs.readVarUInt32();

                    switch ( type ) {
                    case svc_CreateStringTable:
                        ASSERT_GREATER( 1000, size, "Message doesn't fit data buffer" );
                        bs.readBytes( data, size );
                        this->svc_handle_stringtable_create( data, size );
                        break;
                    default:
                        bs.seekForward( size << 3 );
                        break;
                    }
                }

                bs.setPosition(0);

                // apply stringtables
                for ( auto& tbl : proto.string_table().tables() ) {
                    ASSERT_TRUE( stringtables.has_key( tbl.table_name() ), "Unable to find stringtable require for full_packet" );

                    auto& stbl = stringtables.by_key( tbl.table_name() );
                    stbl->update( tbl );
                }

                // handle rest of packet, aka entities
                this->dem_handle_packet( bs, nullptr );
            } else if (p.type != DEM_Packet) {
                dem->set_pos(lpos);
                parse( nullptr );
            }

            if (!trigger) continue;

            // try to get the gamerules packet
            while ( e == nullptr && dem->good() ) {
                // try to find gamerules proxy
                auto ecls = classes.classes.by_key( "CDOTAGamerulesProxy" ).index;
                for ( uint32_t i = 0; i < entities.size(); ++i ) {
                    if ( entities[i] && entities[i]->cls == ecls ) {
                        e = entities[i];
                        break;
                    }
                }

                // parse one to advance state
                parse( nullptr );
            }

            ASSERT_TRUE( e, "Unable to find GamerulesProxy entity" );

            // check if we can get to the seekpoint
            trigger = false;
            float gtime = gtime = e->get( "m_pGameRules.m_fGameTime"_chash )->data.fl;

            if (time - gtime < 61) {
                while (gtime < time) {
                    gtime = e->get( "m_pGameRules.m_fGameTime"_chash )->data.fl;
                    parse( nullptr );
                }
            }

            if (gtime >= time) {
                break;
            }
        }
    }

    void parser::dem_handle_file_header( dem_packet& p ) {
        CDemoFileHeader proto;
        ASSERT_TRUE( proto.ParseFromArray( p.data, p.size ), "Unable to parse protobuf packet" );

/**
 * The buildversion is not available in gameservers hosted on OS X machines
 *
 * We'll not compute the version if we are running within emscripten as this is an easy cause for a unhandled
 * exception within stoi.
 */

#ifndef EMSCRIPTEN
        // Let's abuse game_directory to get the build version
        std::string gamedir = proto.game_directory();

        // Check syntax
        if ( gamedir.length() < 32 ) {
            this->buildnumber = 99999;
            printf("Gamedir syntax changed, using maximum build number.\n");
            return;
        }

        // Try parsing the build number
        try {
            this->buildnumber = std::stoi( gamedir.substr( 30 ) );
        } catch ( std::exception& e ) {
            this->buildnumber = 99999;
            printf("Error determining build number, using maximum.\n");
        }

        ASSERT_TRUE( this->buildnumber >= 1027, "Unsupported replay format" );
#else  /* EMSCRIPTEN */
        this->buildnumber = 99999;
#endif /* EMSCRIPTEN */
    }

    void parser::dem_handle_send_tables( dem_packet& p ) {
        CDemoSendTables proto;
        ASSERT_TRUE( proto.ParseFromArray( p.data, p.size ), "Unable to parse protobuf packet" );

        // Packet contents: Size as varint, Serialized Flattables buffer
        const std::string& buf = proto.data();
        ASSERT_GREATER( buf.size(), 10, "Sendtables packet corrupt" );

        // Read size and verify that we have enough bytes remaining
        uint32_t size, psize;
        uint8_t* data = read_varint32_fast( (uint8_t*)buf.c_str(), size );
        psize         = data - (uint8_t*)buf.c_str();

        ASSERT_GREATER( buf.size() - psize, size, "Sendtables packet corrupt" );
        this->serializers = new flattened_serializer( data, size );
    }

    void parser::dem_handle_packet( bitstream& bs, visitor* v ) {
        // Read packet data
        while ( bs.remaining() > 8 ) {
            alignas( 8 ) char data[70000];
            uint32_t type = bs.readUBitVar();
            uint32_t size = bs.readVarUInt32();

            ASSERT_GREATER( 70000, size, "Message doesn't fit data buffer" );

            switch ( type ) {
            case svc_CreateStringTable:
                bs.readBytes( data, size );
                this->svc_handle_stringtable_create( data, size );
                break;
            case svc_UpdateStringTable:
                bs.readBytes( data, size );
                this->svc_handle_stringtable_update( data, size );
                break;
            case svc_PacketEntities:
                bs.readBytes( data, size );
                this->svc_handle_entities( data, size, (visitor*)v );
                break;
            case GE_Source1LegacyGameEventList:
                bs.readBytes( data, size );
                this->events.load_from_buffer( size, data );
                break;
            case GE_Source1LegacyGameEvent: {
                if ( v ) {
                    bs.readBytes( data, size );

                    CMsgSource1LegacyGameEvent proto;
                    proto.ParseFromArray( data, size );

                    v->on_event( &proto );
                } else {
                    bs.seekForward( size << 3 );
                }
            } break;
            case DOTA_UM_ParticleManager:
            case UM_ParticleManager: {
                bs.readBytes( data, size );
                particles.process_update( data, size );
            } break;
            default:
                ASSERT_TRUE( type < packets.size(), "Unkown type would overflow packet list" );
                if ( v && packets[type] ) {
                    bs.readBytes( data, size );
                    v->on_packet( type, data, size );
                } else {
                    bs.seekForward( size << 3 );
                }
                break;
            }
        }
    }

    void parser::dem_handle_class_info( dem_packet& p ) {
        CDemoClassInfo proto;
        ASSERT_TRUE( proto.ParseFromArray( p.data, p.size ), "Unable to parse protobuf packet" );

        BENCHMARK_START( map_classes );

        // Map all class_ids to their network_name
        for ( auto& c : proto.classes() ) {
            ASSERT_TRUE( (unsigned)c.class_id() == classes.classes.size(), "Invalid id skip" );

            classes->insert( c.class_id(), c.network_name(),
                entity_classes::class_info{constexpr_hash_rt( c.network_name().c_str() ), 0} );

            /** Match prefix */
            auto match = [&]( std::string m1, uint32_t t ) {
                if ( std::mismatch( m1.begin(), m1.end(), c.network_name().begin() ).first == m1.end() ) {
                    classes->by_index( c.class_id() )->type = t;
                }
            };

            match( "CDOTA_Ability", ENT_ABILITY );
            match( "CDOTABaseAbility", ENT_ABILITY );
            match( "CDOTA_BaseNPC", ENT_NPC );
            match( "CDOTA_NPC", ENT_NPC );
            match( "CDOTA_Unit", ENT_UNIT );
            match( "CDOTA_Unit_Hero", ENT_HERO );
            match( "CDOTA_Unit_Hero_Beastmaster_", ENT_UNIT );
            match( "CDOTA_Item", ENT_ITEM );
        }

        BENCHMARK_END( map_classes );

        // Build serializers
        serializers->build( classes );
    }

    void parser::svc_handle_stringtable_create( const char* data, uint32_t size ) {
        CSVCMsg_CreateStringTable proto;
        ASSERT_TRUE( proto.ParseFromArray( data, size ), "Unable to parse protobuf packet" );

        // Ignore duplicate stringtables when seeking, as we handle creation out-of-bounds
        if (!stringtables.has_key(proto.name())) {
            stringtables.insert( stringtables.size(), proto.name(), stringtable( &proto ) );
        }
    }

    void parser::svc_handle_stringtable_update( const char* data, uint32_t size ) {
        CSVCMsg_UpdateStringTable proto;
        ASSERT_TRUE( proto.ParseFromArray( data, size ), "Unable to parse protobuf packet" );

        ASSERT_TRUE( stringtables.has_index( proto.table_id() ), "Trying to update unkown stringtable" );
        auto& tbl = stringtables.by_index( proto.table_id() );
        tbl.value.update( &proto );
    }

    void parser::svc_handle_entities( const char* data, uint32_t size, visitor* v ) {
        CSVCMsg_PacketEntities proto;
        ASSERT_TRUE( proto.ParseFromArray( data, size ), "Unable to parse protobuf packet" );

        bitstream b( proto.entity_data() );

        int32_t idx = -1;

        stringtable& baselines = stringtables.by_key("instancebaseline").value;

        for ( int32_t i = 0; i < proto.updated_entries(); ++i ) {
            // Update entity index
            idx += b.readUBitVar() + 1;

            ASSERT_TRUE( idx >= 0 && idx < BUTTERFLY_MAX_ENTS, "Invalid entity index" );

            // Determine update type
            int32_t etype = 0;
            if ( b.readBool() ) {
                if ( b.readBool() ) {
                    etype = E_DELETE;
                } else {
                    etype = E_LEAVE;
                }
            } else {
                if ( b.readBool() ) {
                    etype = E_CREATE;
                } else {
                    etype = E_UPDATE;
                }
            }

            // Handle update
            switch ( etype ) {
            case E_CREATE: {
                uint32_t cls = b.read( classes.bits() );
                b.read( 17 );      // serial
                b.readVarUInt32(); // unkown

                // Free old entity if applicable
                if ( entities[idx] ) {
                    g_entalloc.free(entities[idx]);
                    entities[idx] = nullptr;
                }

                // Create new
                if ( !entities[idx] ) {
                    entities[idx] = g_entalloc.malloc();
                }

                // Parse entity
                entities[idx]->id       = idx;
                entities[idx]->cls      = cls;
                entities[idx]->cls_hash = classes.classes.by_index( cls )->hash;
                entities[idx]->type     = classes.classes.by_index( cls )->type;
                entities[idx]->set_serializer( &serializers->get( cls ) );

                const std::string bkey = std::to_string(cls);
                if (baselines.has_key(bkey) && !baselines.by_key(bkey).value.empty()) {
                    bitstream b(baselines.by_key(bkey).value);
                    entities[idx]->parse( b );
                }

                entities[idx]->parse( b );

                // Emit event
                if ( v ) v->on_entity( ENT_CREATED, entities[idx] );
            } break;
            case E_UPDATE: {
                ASSERT_TRUE( entities[idx], "Unable to find entity in update" );
                entities[idx]->parse( b );
                if ( v ) v->on_entity( ENT_UPDATED, entities[idx] );
            } break;
            case E_LEAVE: {
            } break;
            case E_DELETE: {
                if ( entities[idx] ) {
                    if ( v ) v->on_entity( ENT_DELETED, entities[idx] );
                    g_entalloc.free( entities[idx] );
                }

                entities[idx] = nullptr;
            } break;
            }
        }
    }
} /* butterfly */
