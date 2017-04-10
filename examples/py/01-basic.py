"""
 * @file 01-basic.py
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
"""

import butterpy, sys

if __name__ == "__main__":
    # Check arguments
    if len(sys.argv) != 2:
        print("Usage: python 01-basic.py <path/to/replay>")
        sys.exit(1)

    # Create an empty visitor
    v = butterpy.visitor()

    # Create a parser
    p = butterpy.parser()

    # Open a replay, second argument is a progress callback function
    p.open(sys.argv[1], None)

    # Parse all the packets
    p.parse_all(v)
