// In typical js fashion, we load our module
var butterfly = require("./butterflyjs1.js");
var parser = null;

// In order to get data out of the replay, we need to define a visitor that will
// be called each time a state advances.
//
// There are four major callbacks you can use:
// * on_packet: Protobuf packets
// * on_state: Parser state, can be used to detemine the point when entities are available
// * on_entity: Callback for each entity change
// * on_tick: Callback for each tick change
//
// You cannot leave any of these callbacks out, even if you don't use them
var visitor = {
    // Counter for the number of combatlog packets we've seen
    clog_packets_received: 0,

    // We only want to print the first item
    printed: false,

    on_packet: function(id, data) {
        // In order to get a callback for a packet, you'll have to
        // do parser.require(PacketID).
        // Data is a normal js object that has all the protobuf data in it

        // The huge downside of using the javascript API is that all the protobufs
        // need to be converted to JS objects using reflection.
        // This is rather slow and consits of copying around a lot of memory.
        // Getting the combatlog data can add up to 10 seconds to a parse.

        if (id == 554) {
            // This is a combatlog packet
            this.clog_packets_received++;

            if (this.clog_packets_received == 1) {
                console.log("Our first combatlog packet looks like this:");
                console.log(data);
            }
        }
    },

    on_state: function(state) {
        // State is an enum
        console.log("Switched parser state to: "+state);

        if (state == 3) {
            // Log number of combatlog packets we've seen since we started parsing
            console.log("We've seen "+this.clog_packets_received+" combatlog packets");
        }
    },

    on_entity: function(state, entity) {
        // Getting the entity data is a bit more evolved because the underlying structures
        // have been optimized for C++ usage.

        // Let's look at all the "entity_created" events and print the name of the entity
        if (state == butterfly.entity_state.ENT_CREATED) {
            // In this example, we are going to focus on all newly created items
            if (entity.type() != butterfly.entity_types.ENT_ITEM) {
                return;
            }

            // Get some meta information about the entity
            var idx = entity.id(); // index of the entity in the global entity list
            var clazz = entity.cls(); // class id
            var clazz_name = parser.class_name(clazz); // class name

            console.log("Item entity created and idx "+entity.id()+" using class "+clazz_name);

            // We also want to get some more information about the available properties.
            // If you want to see all attributes, the handy function "spew" will print all the
            // data to the console as an ascii table.

            // Uncomment this line to get the table
            // entity.spew();

            // The 'in-code' way to iterate over all attribute is by using `properties` to get a list of
            // property hashes that can be resolved to their value.
            if (!this.printed) {
                this.printed = true;
                //console.log("Properties of the first item:");

                var props = entity.properties();
                for (var i = 0; i < props.size(); ++i) {
                    // get a property hash and it's handle
                    var prop_hash = props.get(i);
                    var prop = entity.get_by_hash(prop_hash);

                    // Additional information about a property is stored in it's serializer.
                    // We'll access that to get the property name:
                    var prop_name = prop.info().name;

                    console.log(prop_name + "\t" + prop.value());
                }
            }

            // The other way to get a property is by using it's key.
            // In this case we'll get the value of "m_pEntity.m_nameStringableIndex", and resolve it
            // in the corresponding stringtable.
            var s_name_index = entity.get("m_pEntity.m_nameStringableIndex").value();

            // Now we'll get the name from the stringtable
            if (s_name_index != -1) {
                var entityNamesTable = parser.stringtables().by_key("EntityNames").value();
                var entityName = entityNamesTable.by_index(s_name_index).key;
                console.log("Stringtable name: "+entityName);
            } else {
                console.log("Stringtable name: Unkown");
            }
        }
    },

    // We don't use this, but we are still going to define it as emscripten throws an error
    // otherwise
    on_tick: function(tick) {},

    // Called with a floating point value from (0 - 100) whenever the parser made progress
    on_progress: function(prec) {}
};

// We need to call visitor.implement to add all the emscripten specific stuff
// on top of our callbacks.
var cvisitor = butterfly.visitor.implement(visitor);

// Now we create a new parser and open a replay.
// Because the filesystem in emscripten depends on the type of platform (Browser / Node)
// you run on, there are three different prefixes depending on where your replay is:
// * /node_root/ -> Equals /
// * /node_cwd/  -> Equals `cwd`
// * /web_root/  -> Equals / in TEMPFS

parser = new butterfly.parser();
parser.open("/node_cwd/2179252492.dem", cvisitor);

// this line enables callbacks for combatlog packets
// you can get the packet IDs from the protobufs at <Link>
parser.require(554);

// this function instructs the parser to parse all packets in the replay and use our visitor as a callback
parser.parse_all(cvisitor);
