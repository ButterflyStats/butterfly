/**
 * @file entity_classes.hpp
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

#ifndef BUTTERFLY_ENTITY_CLASSES_HPP
#define BUTTERFLY_ENTITY_CLASSES_HPP

#include <string>
#include <cmath>
#include <cstdint>

#include <butterfly/util_assert.hpp>
#include <butterfly/util_dict.hpp>

namespace butterfly {
    /** Different types of entities */
    enum entity_types {
        ENT_DEFAULT = 0, // Any not fitting 1-5
        ENT_ABILITY = 1, // CDOTA_Ability*, CDOTABaseAbility
        ENT_NPC     = 2, // CDOTA_BaseNPC, CDOTA_NPC
        ENT_HERO    = 3, // CDOTA_Unit_Hero
        ENT_ITEM    = 4, // CDOTA_Item
        ENT_UNIT    = 5  // CDOTA_Unit
    };

    /** Stores all possible entity classes */
    struct entity_classes {
        /** Information about a single class */
        struct class_info {
            /** Class name hash, fnv1a */
            uint64_t hash;
            /**
             * Class entity type.
             *
             * Types are not stored in the replay but provided by butterfly as a convinience to filter
             * entities. Report any classification errors as bugs.
             */
            uint8_t type;
        };

        /** List of networked classes with their id mapped to the name */
        dict<class_info> classes;

        /** Shortcut to classes.classes */
        dict<class_info>* operator->() { return &classes; }

        /** Returns true if class is networked */
        bool is_networked( const std::string& cls ) { return classes.has_key( cls ); }

        /** Returns id of class */
        uint32_t class_id( const std::string& cls ) { return classes.by_key( cls ).index; }

        /** Returns number of bits to read for entity classes */
        uint8_t bits() { return std::ceil( std::log2( classes.size() ) ); }
    };
} /* butterfly */

#endif /* BUTTERFLY_ENTITY_CLASSES_HPP */
