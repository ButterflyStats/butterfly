/**
 * @file particle.cpp
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
#include <cstdint>
#include <butterfly/particle.hpp>
#include <butterfly/resources.hpp>
#include <butterfly/proto/usermessages.pb.h>

#define ASSIGN_IF_PROTO( _proto_, _field, _default ) ( _proto_.has_##_field() ) ? ( _proto_._field() ) : ( _default );

namespace butterfly {
    static particle::control_point& find_or_create_cp( uint32_t idx, particle& p ) {
        for ( auto& cp : p.cpoints ) {
            if ( cp.idx == idx ) {
                return cp;
            }
        }

        particle::control_point cp;
        cp.idx = idx;
        cp.x   = 0.0f;
        cp.y   = 0.0f;
        cp.z   = 0.0f;
        cp.f_x = 0.0f;
        cp.f_y = 0.0f;
        cp.f_z = 0.0f;

        p.cpoints.push_back( cp );
        return p.cpoints.back();
    }

    void particle_manager::process_update( char* data, uint32_t size ) {
        CUserMsg_ParticleManager proto;
        proto.ParseFromArray( data, size );

        switch ( proto.type() ) {
        // Create a new particle
        case GAME_PARTICLE_MANAGER_EVENT_CREATE: {
            particle p;
            p.name              = resource_lookup( proto.create_particle().particle_name_index() );
            p.name_idx          = proto.create_particle().particle_name_index();
            p.attach_type       = ASSIGN_IF_PROTO( proto.create_particle(), attach_type, 0 );
            p.ehandle           = ASSIGN_IF_PROTO( proto.create_particle(), entity_handle, 0 );
            p.ehandle_modifiers = ASSIGN_IF_PROTO( proto.create_particle(), entity_handle_for_modifiers, 0 );

            particles[proto.index()] = p;
        } break;

        // Release the given particle index
        case GAME_PARTICLE_MANAGER_EVENT_RELEASE: {
            auto it = particles.find( proto.index() );
            if ( it != particles.end() ) {
                particles.erase( it );
            }
        } break;

        // Destroy the particle effect
        case GAME_PARTICLE_MANAGER_EVENT_DESTROY: {
            auto it = particles.find( proto.index() );
            if ( it != particles.end() ) {
                it->second.rendered = false;
            }
        } break;

        // Update particle effect control point
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE: {
            auto it = particles.find( proto.index() );
            if ( it == particles.end() )
                return;

            auto cp = find_or_create_cp( proto.update_particle().control_point(), it->second );
            cp.x    = ASSIGN_IF_PROTO( proto.update_particle().position(), x, cp.x );
            cp.y    = ASSIGN_IF_PROTO( proto.update_particle().position(), y, cp.y );
            cp.z    = ASSIGN_IF_PROTO( proto.update_particle().position(), z, cp.z );
        } break;

        // Update particle effect entity
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE_ENT: {
            auto it = particles.find( proto.index() );
            if ( it == particles.end() )
                return;

            // @todo: attribute should probably attach to a control point
            auto& part       = it->second;
            part.ehandle     = ASSIGN_IF_PROTO( proto.update_particle_ent(), entity_handle, part.ehandle );
            part.attach_type = ASSIGN_IF_PROTO( proto.update_particle_ent(), attach_type, part.attach_type );
            part.attachment  = ASSIGN_IF_PROTO( proto.update_particle_ent(), attachment, part.attachment );
            part.include_wearables =
                ASSIGN_IF_PROTO( proto.update_particle_ent(), include_wearables, part.include_wearables );
        } break;

        // @todo
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE_FORWARD:
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE_ORIENTATION:
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE_FALLBACK:
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE_OFFSET:
        case GAME_PARTICLE_MANAGER_EVENT_DESTROY_INVOLVING:
        case GAME_PARTICLE_MANAGER_EVENT_LATENCY:
        case GAME_PARTICLE_MANAGER_EVENT_SHOULD_DRAW:
        case GAME_PARTICLE_MANAGER_EVENT_FROZEN:
        case GAME_PARTICLE_MANAGER_EVENT_CHANGE_CONTROL_POINT_ATTACHMENT:
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE_ENTITY_POSITION:
        case GAME_PARTICLE_MANAGER_EVENT_SET_FOW_PROPERTIES:
        case GAME_PARTICLE_MANAGER_EVENT_SET_TEXT:
        case GAME_PARTICLE_MANAGER_EVENT_SET_SHOULD_CHECK_FOW:
        case GAME_PARTICLE_MANAGER_EVENT_SET_CONTROL_POINT_MODEL:
        case GAME_PARTICLE_MANAGER_EVENT_SET_CONTROL_POINT_SNAPSHOT:
        case GAME_PARTICLE_MANAGER_EVENT_SET_TEXTURE_ATTRIBUTE:
        case GAME_PARTICLE_MANAGER_EVENT_SET_SCENE_OBJECT_GENERIC_FLAG:
        case GAME_PARTICLE_MANAGER_EVENT_SET_SCENE_OBJECT_TINT_AND_DESAT:
        case GAME_PARTICLE_MANAGER_EVENT_DESTROY_NAMED:
        case GAME_PARTICLE_MANAGER_EVENT_SKIP_TO_TIME:
        case GAME_PARTICLE_MANAGER_EVENT_CAN_FREEZE:
        case GAME_PARTICLE_MANAGER_EVENT_SET_NAMED_VALUE_CONTEXT:
        case GAME_PARTICLE_MANAGER_EVENT_UPDATE_TRANSFORM:
        case GAME_PARTICLE_MANAGER_EVENT_FREEZE_TRANSITION_OVERRIDE:
        case GAME_PARTICLE_MANAGER_EVENT_FREEZE_INVOLVING:
            break;
        }
    }
} /* butterfly */

#undef ASSIGN_IF_PROTO
