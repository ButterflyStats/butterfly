/**
 * @file 01-basic.cpp
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

#include <cstdio>
#include <butterfly/butterfly.hpp>
#include <butterfly/visitor.hpp>

using namespace butterfly;

/** Implemen overloads here */
class your_visitor : public visitor {};

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: 01-basic <replay>\n");
        return 1;
    }

    your_visitor v;

    BENCHMARK_START(Overall);
    butterfly::parser p;
    p.open(argv[1]);
    p.parse_all(&v);
    BENCHMARK_END(Overall);

    return 0;
}
