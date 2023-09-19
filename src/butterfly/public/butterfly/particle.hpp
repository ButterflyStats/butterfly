/**
 * @file particle.hpp
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

#ifndef BUTTERFLY_PARTICLE_HPP
#define BUTTERFLY_PARTICLE_HPP

#include <vector>
#include <unordered_map>
#include <cstdint>

#include "resource_manifest.hpp"

namespace butterfly {
    /** Particle being rendered */
    struct particle {
        /** Initializes particle with default arguments */
        particle()
            : name( "" ), name_idx( 0 ), ehandle( 0 ), ehandle_modifiers( 0 ), attach_type( 0 ), attachment( 0 ),
              include_wearables( false ), rendered( true ) {}

        /** Particle control point, coords can be relative to world or local */
        struct control_point {
            uint8_t idx;
            float x, y, z;
            float f_x, f_y, f_z;
        };

        /** List of control points */
        std::vector<control_point> cpoints;
        /** Particle name */
        std::string name;
        /** Name Index */
        uint64_t name_idx;
        /** Entity handle, might be 0 */
        uint32_t ehandle;
        /** Entity handle for modifiers, might be 0 */
        uint32_t ehandle_modifiers;
        /** Entity attachment type */
        uint8_t attach_type;
        /** Entity attachment pos */
        uint8_t attachment;
        /** Include effect on wearables? */
        bool include_wearables;
        /** Is the particle active? */
        bool rendered;
    };

    /** Particle Manager */
    class particle_manager {
    public:
        typedef std::unordered_map<uint32_t, particle> storage_t;

        /** Value type */
        typedef storage_t::value_type value_type;
        /** Size type */
        typedef typename storage_t::size_type size_type;
        /** Iterator */
        typedef typename storage_t::iterator iterator;
        /** Const iterattor */
        typedef typename storage_t::const_iterator const_iterator;

        /** Return iterator to beginning of particle map */
        iterator begin() { return particles.begin(); }

        /** Return iterator to end of particle map */
        iterator end() { return particles.end(); }

        /** Return iterator to idx position or end */
        iterator find( uint32_t idx ) { return particles.find( idx ); }

        /** Number of active particles */
        size_type size() { return particles.size(); }

        /** Clear all particles */
        void clear() { particles.clear(); }

        /** Process particle manager updates */
        void process_update( const resource_manifest& manifest, char* data, uint32_t size );

    private:
        storage_t particles;
    };
}

#endif /* BUTTERFLY_PARTICLE_HPP */
