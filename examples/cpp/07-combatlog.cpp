/**
 * @file 07-combatlog.cpp
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

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <cstdio>

#include <butterfly/butterfly.hpp>
#include <butterfly/visitor.hpp>
#include <butterfly/proto/dota_gcmessages_common.pb.h>

using namespace butterfly;

// Parser
butterfly::parser p;

// Combatlog helper
butterfly::combatlog cmb;

/** Event Visitor */
class combatlog_visitor : public visitor {
public:
    virtual void on_packet( uint32_t type, char* data, uint32_t size ) {
        if (type != DOTA_UM_CombatLogDataHLTV)
            return;

        cmb.add_from_buffer(data, size);

        // Access the last entry using:
        // ----------------------------
        // cmb.at(cmb.size()-1)

        // Access all entries using:
        // -------------------------
        // for (uint32_t i = 0; i < cmb.size(); ++i) {
        //   cmb.at(i);
        // }
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: 07-combatlog <replay>\n");
        return 1;
    }

    combatlog_visitor v;

    // tell the combatlog to store the last 100.000 entries
    cmb.set_retention(100000);

    // parse everything
    p.open(argv[1]);
    p.require(DOTA_UM_CombatLogDataHLTV);
    p.parse_all(&v);

    // Filter entries
    // You should just use max() here but this way is nice to show some of whats possible
    /*auto val = cmb.filter()
                  .where( [](const combatlog::entry& e) { return e.type == combatlog::T_DAMAGE; })
                  .orderBy( [](const combatlog::entry& e) { return e.value; })
                  .select( [](const combatlog::entry& e) { return e.value; })
                  .reverse()
                  .elementAt(1);*/

    //std::cout << "Highest instance of single target damage in the last 100.000 combatlog entries was: " << val << std::endl;

    return 0;
}
