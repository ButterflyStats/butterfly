"""
 * @file 03-props.py
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

seen = []
p = None # Parser

class visitor(butterpy.visitor):
    def on_entity(self, state, entity):
        """ Called when an entity changes it's state (create, update, delete) """

        # Only handle entities that are being created
        if state != butterpy.visitor.ENT_CREATED:
            return

        # Only handle each entity class once
        if entity.cls() in seen:
            return

        # Append the entity class so we don't dump the same class multiple times (e.g. creeps being spawend)
        seen.append(entity.cls())

        # The code below get's the serialization info
        # The underlying structure is mostly compatible with pythons dictionary, but as we need to geth meta attributes
        # (key and index), we can't use [i] but have to use by_index.
        #
        # _.by_index(i).value is equal to _[i]. by_index and by_key carry .key and .index for the metadata.
        # _.by_index(i).index == i && _.by_index(k).key == k. k is always a string, i an unsigned integer

        ename = p.entity_classes.data.by_index(entity.cls()).key


        print("Entity {:s} ({:d})".format(ename, entity.cls()))
        print("=======================")

        # Print all entity attributes
        for prop in entity.properties:
            print("{:s} - {}".format(entity.properties[prop].info.name, entity.properties[prop].value()))

        # Newline after each entity
        print("")

if __name__ == "__main__":
    # Check arguments
    if len(sys.argv) != 2:
        print("Usage: python 01-basic.py <path/to/replay>")
        sys.exit(1)

    # Create an empty visitor
    v = visitor()

    # Create a parser
    p = butterpy.parser()

    # Open a replay, second argument is a progress callback function
    p.open(sys.argv[1], None)

    # Parse all the packets
    p.parse_all(v)
