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
 *
 * @par Description
 *    Combatlog related data structures.
 */

#ifndef BUTTERFLY_COMBATLOG_HPP
#define BUTTERFLY_COMBATLOG_HPP

#include <cstdint>
#include <butterfly/util_ringbuffer.hpp>

namespace butterfly {
    /**
     * Provides combatlog parsing, storage and filtering.
     *
     * To use this class, set a retention via the set_retition methods, specifing the number of
     * entries that should be kept in memory. Parsing the Combatlog entries does not happen by itself,
     * you'll have to subscribe to DOTA_UM_CombatLogDataHLTV and supplie it's contents to add_from_buffer.
     *
     * For an example see examples/07-combatlog.cpp
     */
    class combatlog {
    public:
        /** Possible entry flags */
        enum flags {
            ATTACKER_IS_ILLUSION  = ( 1 << 0 ),
            ATTACKER_IS_HERO      = ( 1 << 1 ),
            TARGET_IS_ILLUSION    = ( 1 << 3 ),
            TARGET_IS_HERO        = ( 1 << 4 ),
            TARGET_IS_BUILDING    = ( 1 << 5 ),
            TARGET_IS_SELF        = ( 1 << 6 ),
            VISIBLE_RADIANT       = ( 1 << 7 ),
            VISIBLE_DIRE          = ( 1 << 8 ),
            ABILITY_TOGGLED_ON    = ( 1 << 9 ),
            ABILITY_TOGGLED_OFF   = ( 1 << 10 ),
            ABILITY_IS_ULTIMATE   = ( 1 << 11 ),
            ABILITY_IS_HEAL       = ( 1 << 12 ),
            HIDDEN_MODIFIER       = ( 1 << 13 ),
            INVISIBILITY_MODIFIER = ( 1 << 14 ),
            FLAGS_MAX             = ( 1 << 14 )
        };

        /**
         * Possible entry types.
         * Determines which fields are actually filled in the entry struct.
         */
        enum type {
            T_DAMAGE             = 0,
            T_HEAL               = 1,
            T_MODIFIER_ADD       = 2,
            T_MODIFIER_REMOVE    = 3,
            T_DEATH              = 4,
            T_ABILITY            = 5,
            T_ITEM               = 6,
            T_LOCATION           = 7,
            T_GOLD               = 8,
            T_GAME_STATE         = 9,
            T_XP                 = 10,
            T_PURCHASE           = 11,
            T_BUYBACK            = 12,
            T_ABILITY_TRIGGER    = 13,
            T_PLAYERSTATS        = 14,
            T_MULTIKILL          = 15,
            T_KILLSTREAK         = 16,
            T_TEAM_BUILDING_KILL = 17,
            T_FIRST_BLOOD        = 18,
            T_MODIFIER_REFRESH   = 19,
            T_NEUTRAL_CAMP_STACK = 20,
            T_PICKUP_RUNE        = 21,
            T_REVEALED_INVISIBLE = 22,
            T_HERO_SAVED         = 23,
            T_MANA_RESTORED      = 24,
            T_HERO_LEVELUP       = 25,
            T_BOTTLE_HEAL_ALLY   = 26,
            T_ENDGAME_STATS      = 27,
            T_INTERRUPT_CHANNEL  = 28,
            T_ALLIED_GOLD        = 29,
            T_AEGIS_TAKEN        = 30
        };

        /** A single combatlog entry. Fields are assigned based on the type. */
        struct entry {
            uint8_t type;
            uint32_t flags;

            uint16_t target_name;
            uint16_t target_source_name;
            uint16_t attacker_name;
            uint16_t damage_source_name;
            uint16_t inflictor_name;
            uint32_t value;
            int16_t health;
            float timestamp;
            float stun_duration;
            float slow_duration;
            uint8_t ability_level;
            float location_x;
            float location_y;
            uint8_t gold_reason;
            float timestamp_raw;
            float modifier_duration;
            uint8_t xp_reason;
            uint16_t last_hits;
            uint8_t attacker_team;
            uint8_t target_team;
            uint8_t obs_wards_placed;
            uint16_t assist_player0;
            uint16_t assist_player1;
            uint16_t assist_player2;
            uint16_t assist_player3;
            uint8_t stack_count;
            uint8_t neutral_camp_type;
            uint8_t rune_type;
            uint8_t attacker_hero_level;
            uint8_t target_hero_level;
            uint16_t xpm;
            uint16_t gpm;
            uint32_t event_location;
            uint32_t damage_type;
            uint32_t damage_category;
            uint32_t networth;
            uint32_t building_type;
            float modifier_elapsed_duration;
        };

    private:
        /** Keeps track of recent entries */
        ringbuffer<entry> entries;

    public:
        /** Sets number of entries to keep in history, can not shrink, default is 128 */
        void set_retention( uint32_t num ) { entries.resize( num ); }

        /** Return the number of entries stored */
        uint32_t size() { return entries.size(); }

        /** Returns the entry at the given position. Return value for idx >= size is undefined. */
        entry* at( uint32_t idx ) { return &entries.at( idx ); }

        /** Adds entry from a DOTA_UM_CombatLogDataHLTV buffer */
        void add_from_buffer( char* buf, uint32_t size );
    };
} /* butterfly */

#endif /* BUTTERFLY_COMBATLOG_HPP */
