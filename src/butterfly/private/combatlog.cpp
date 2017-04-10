/**
 * @file combatlog.hpp
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

#include <cstdint>
#include <butterfly/combatlog.hpp>
#include <butterfly/proto/dota_gcmessages_common.pb.h>

#define CHECK_FLAG( _name_, _flag_ )                                                                                   \
    do {                                                                                                               \
        if ( proto.has_##_name_() && proto._name_() )                                                                  \
            flags |= _flag_;                                                                                           \
    } while ( 0 )

#define ASSIGN( _name_ )                                                                                               \
    do {                                                                                                               \
        if ( proto.has_##_name_() )                                                                                    \
            e._name_ = proto._name_();                                                                                 \
    } while ( 0 )

namespace butterfly {
    void combatlog::add_from_buffer( char* buf, uint32_t size ) {
        CMsgDOTACombatLogEntry proto;
        proto.ParseFromArray( buf, size );

        uint32_t flags = 0;

        CHECK_FLAG( is_attacker_illusion, ATTACKER_IS_ILLUSION );
        CHECK_FLAG( is_attacker_hero, ATTACKER_IS_HERO );
        CHECK_FLAG( is_target_illusion, TARGET_IS_ILLUSION );
        CHECK_FLAG( is_target_hero, TARGET_IS_HERO );
        CHECK_FLAG( is_visible_radiant, VISIBLE_RADIANT );
        CHECK_FLAG( is_visible_dire, VISIBLE_DIRE );
        CHECK_FLAG( is_ability_toggle_on, ABILITY_TOGGLED_ON );
        CHECK_FLAG( is_ability_toggle_off, ABILITY_TOGGLED_OFF );
        CHECK_FLAG( hidden_modifier, HIDDEN_MODIFIER );
        CHECK_FLAG( is_target_building, TARGET_IS_BUILDING );
        CHECK_FLAG( is_heal_save, ABILITY_IS_HEAL );
        CHECK_FLAG( is_ultimate_ability, ABILITY_IS_ULTIMATE );
        CHECK_FLAG( target_is_self, TARGET_IS_SELF );
        CHECK_FLAG( invisibility_modifier, INVISIBILITY_MODIFIER );

        entry e;
        e.type  = proto.type();
        e.flags = flags;

        ASSIGN( target_name );
        ASSIGN( target_source_name );
        ASSIGN( attacker_name );
        ASSIGN( damage_source_name );
        ASSIGN( inflictor_name );
        ASSIGN( value );
        ASSIGN( health );
        ASSIGN( timestamp );
        ASSIGN( stun_duration );
        ASSIGN( slow_duration );
        ASSIGN( ability_level );
        ASSIGN( location_x );
        ASSIGN( location_y );
        ASSIGN( gold_reason );
        ASSIGN( timestamp_raw );
        ASSIGN( modifier_duration );
        ASSIGN( xp_reason );
        ASSIGN( last_hits );
        ASSIGN( attacker_team );
        ASSIGN( target_team );
        ASSIGN( obs_wards_placed );
        ASSIGN( assist_player0 );
        ASSIGN( assist_player1 );
        ASSIGN( assist_player2 );
        ASSIGN( assist_player3 );
        ASSIGN( stack_count );
        ASSIGN( neutral_camp_type );
        ASSIGN( rune_type );
        ASSIGN( attacker_hero_level );
        ASSIGN( target_hero_level );
        ASSIGN( xpm );
        ASSIGN( gpm );
        ASSIGN( event_location );
        ASSIGN( damage_type );
        ASSIGN( damage_category );
        ASSIGN( networth );
        ASSIGN( building_type );
        ASSIGN( modifier_elapsed_duration );

        entries.insert( e );
    }
}

#undef CHECK_FLAG
#undef ASSIGN
