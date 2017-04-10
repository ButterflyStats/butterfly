/**
 * @file 03-props.cpp
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
#include <set>
#include <cstdio>
#include <butterfly/butterfly.hpp>
#include <butterfly/visitor.hpp>

using namespace butterfly;

butterfly::parser p;
std::set<uint16_t> seen;

class your_visitor : public visitor {
public:
    virtual void on_entity(entity_state state, entity* ent) final {
        if (state != ENT_DELETED)
            return;

        auto it = seen.find(ent->cls);
        if (it != seen.end())
            return;

        // First time this entity has been deleted
        seen.insert(ent->cls);
        std::cout << "Entity deleted: " << p->classes->by_index(ent->cls).key << " (" << ent->cls << ")" << std::endl;

        std::cout << "=========================" << std::endl;
        for (auto &p : ent->properties) {
            std::cout << p.second->info->name << " " << p.second->as_string() << std::endl;
        }
        std::cout << std::endl;
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: 03-props <replay>\n");
        return 1;
    }

    your_visitor v;

    BENCHMARK_START(Overall);
    p.open(argv[1]);
    p.parse_all(&v);
    BENCHMARK_END(Overall);

    return 0;
}
