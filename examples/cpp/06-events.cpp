/**
 * @file 06-events.cpp
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

using namespace butterfly;

// Parser
butterfly::parser p;

// Event list
std::map<std::string, uint32_t> events;

/** Event Visitor */
class event_visitor : public visitor {
public:
    virtual void on_event( CMsgSource1LegacyGameEvent* event ) {
        auto descriptor = p->events.by_id(event->eventid());

        auto it = events.find(descriptor->name);
        if (it == events.end()) {
            events[descriptor->name] = 1;
        } else {
            events[descriptor->name] += 1;
        }
    }
};

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: 06-events <replay>\n");
        return 1;
    }

    event_visitor v;

    // parse everything
    p.open(argv[1]);
    p.parse_all(&v);

    // spit out how often each event occurred
    for (auto &e : events) {
        std::cout << e.first << '\t' << e.second << std::endl;
    }

    return 0;
}
