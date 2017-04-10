/**
 * @file 04-deathmap.cpp
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

#include <iostream>
#include <cstdio>
#include <butterfly/butterfly.hpp>
#include <butterfly/visitor.hpp>
#include <butterfly/util_chash.hpp>

using namespace butterfly;

/** Player dead? */
std::vector<bool> dead;

/** Our visitor */
class death_visitor : public visitor {
public:
    virtual void on_entity(entity_state state, entity* e) final {
        if (state != ENT_UPDATED)
            return;

        // Looking at a hero
        if (e->type == ENT_HERO) {
            int64_t pid = e->get("m_iPlayerID"_chash)->data.i32;

            // illu?
            if (e->get("m_hReplicatingOtherHeroModel")->data.u32 != 16777215)
                return;

            if (!dead.at(pid) && e->get("m_lifeState")->data.u32 == 1) {
                std::cout << "Player "  << " (" << pid << ") has died, game tick: " << p->tick
                    << ", Hero: " << p->classes->by_index(e->cls).key << std::endl;
                dead.at(pid) = 1;
            } else if (e->get("m_lifeState")->data.u32 != 1) {
                dead.at(pid) = 0;
            }
        }
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: 04-deathmap <replay>\n");
        return 1;
    }

    death_visitor v;
    dead.resize(100, false);

    parser p;

    BENCHMARK_START(Overall);
    p.open(argv[1]);
    p.parse_all(&v);
    BENCHMARK_END(Overall);

    return 0;
}
