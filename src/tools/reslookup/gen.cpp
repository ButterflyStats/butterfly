/**
 * @file gen.cpp
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

#include <string>
#include <unordered_map>

#include <butterfly/util_assert.hpp>
#include <butterfly/util_vpk.hpp>

#include <butterfly/trie/tx.hpp>

#include <private/util_string.hpp>
#include <private/util_murmur.hpp>

// Initial hash seed
#define MHASH_SEED 0xEDABCDEF

using namespace butterfly;
using namespace tx_tool;

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: reslookup-gen <path to dota> <type>\n");
        printf("Example: reslookup-gen /Steam/steamapps/common/dota 2 beta/game/dota/ [prefix|hash|json]\n");
        return 1;
    }

    // Open pak
    std::string path = std::string(argv[1])+"pak01";
    vpk* v = vpk_from_path(path.c_str());

    // Generate a list of entries we wanna hash
    std::vector<std::string> entries;

    for (auto &e : v->entries) {
        switch (e.type) {
        case VPK_RES_MDL:
        case VPK_RES_PCF: {
            // generate hash
            std::string fpath = std::string(e.folder)+"/"+e.name+"."+e.extension;
            replace_all(fpath, "materials/", "");
            replace_all(fpath, "vmdl_c", "vmdl");
            replace_all(fpath, "vpcf_c", "vpcf");

            entries.push_back(std::move(fpath));
        } break;
        default: break;
        }
    }

    // Build a prefix map
    tx trie;
    trie.build(entries, "trie.txt");
    trie.read("trie.txt");

    // Hash?
    if (strcmp(argv[2], "hash") == 0) {
        for (auto &e: entries) {
            uint64_t hash = MurmurHash64(e.c_str(), e.size(), MHASH_SEED);
            size_t plen = 0;
            uint64_t pidx = trie.prefixSearch(e.c_str(), e.size(), plen);

            std::cout << "ENTRY(" << hash << "u, " << pidx << ");" << std::endl;
        }
    }

    // Prefix lookup?
    if (strcmp(argv[2], "prefix") == 0) {
        FILE *fp = fopen("trie.txt", "rb");
        ASSERT_TRUE(fp, "Can't open trie.txt");

        fseek(fp, 0, SEEK_END);
        size_t fileSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        unsigned char *data = new unsigned char[fileSize];
        fread(data, fileSize, 1, fp);
        fclose(fp);

        for (uint32_t i = 0; i < fileSize; ++i) {
            if (i == 0 || i % 20 == 0) {
                printf("\n\t");
            }

            unsigned char c = data[i];

            if ((i+1) < fileSize)
                printf("0x%02x,", c);
            else
                printf("0x%02x", c);
        }
    }

    // Generate json?
    if (strcmp(argv[2], "json") == 0) {
        // write generation timestamp
        std::cout << "{" << std::endl;
        std::cout << "\t\"time\": " << time(NULL) << "," << std::endl;
        std::cout << "\t\"entries\": {" << std::endl;

        for (auto &e: entries) {
            uint64_t hash = MurmurHash64(e.c_str(), e.size(), MHASH_SEED);
            std::cout << "\t\t\"" << hash << "\": \"" << e.c_str() << "\"," << std::endl;
        }

        std::cout << "\t\t\"default\": \"unkown\"" << std::endl;
        std::cout << "\t}" << std::endl << "}";
    }

    delete v;
    return 0;
}
