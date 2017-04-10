/**
 * @file util_huffman.hpp
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

#ifndef BUTTERFLY_UTIL_HUFFMAN_HPP
#define BUTTERFLY_UTIL_HUFFMAN_HPP

#include <iomanip>
#include <queue>
#include <unordered_map>
#include <vector>
#include <cstddef>

namespace butterfly {
    /** Node Interface */
    struct HuffmanNode {
        const uint32_t weight;
        const uint32_t num;

        HuffmanNode( uint32_t weight, uint32_t num ) : weight( weight ), num( num ) {}

        virtual ~HuffmanNode(){};
    };

    /** Branch node */
    struct HuffmanBranch : public HuffmanNode {
        HuffmanNode* left;
        HuffmanNode* right;

        HuffmanBranch( HuffmanNode* l, HuffmanNode* r, uint32_t num )
            : HuffmanNode( l->weight + r->weight, num ), left( l ), right( r ) {}

        virtual ~HuffmanBranch() final {
            delete left;
            delete right;
        }
    };

    /** Leaf node */
    template <typename Value>
    struct HuffmanLeaf : public HuffmanNode {
        Value v;

        HuffmanLeaf( uint32_t weight, Value v, uint32_t num ) : HuffmanNode( weight, num ), v( v ){};
    };

    /** Huffman compare template */
    template <typename T>
    int32_t huffman_compare( HuffmanNode* left, HuffmanNode* right );

    /**
     * Simple HuffmanCoding implementation.
     * See wikipedia for an in-depth explanation.
     */
    template <typename Value>
    class huffman {
    public:
        /** Build the huffman tree from given values and frequencies */
        void build_tree( const std::vector<Value>& items ) {
            // Valve's Huffman-Tree uses a variation which takes the node number into account
            uint32_t num = 0;

            // Priority queue to sort items by their relative weight
            std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, decltype( &huffman_compare<Value> )> queue(
                &huffman_compare<Value> );

            // Add all items
            for ( auto& v : items ) {
                queue.push( new HuffmanLeaf<Value>( v.weight, v, num++ ) );
            }

            // Branch each leaf until only a single one remains
            while ( queue.size() > 1 ) {
                HuffmanNode* left = queue.top();
                queue.pop();

                HuffmanNode* right = queue.top();
                queue.pop();

                HuffmanNode* branch = new HuffmanBranch( left, right, num++ );
                queue.push( branch );
            }

            tree = queue.top();
        }

        /** Prints all tree codes */
        void print_tree( HuffmanNode* node = nullptr, std::string prefix = "" ) {
            if ( !node )
                node = tree;

            if ( const HuffmanLeaf<Value>* lf = dynamic_cast<HuffmanLeaf<Value>*>( node ) ) {
                printf ("%20s: %-20s\n", lf->v.name.c_str(), prefix.c_str());
            } else if ( const HuffmanBranch* in = dynamic_cast<HuffmanBranch*>( node ) ) {
                std::string leftPrefix = prefix;
                leftPrefix += "0";
                print_tree( in->left, leftPrefix );

                std::string rightPrefix = prefix;
                rightPrefix += "1";
                print_tree( in->right, rightPrefix );
            }
        }

        /** Prints tree as decimal */
        void print_tree_dec( HuffmanNode* node = nullptr, uint32_t dec = 0 ) {
            if ( !node )
                node = tree;

            if ( const HuffmanLeaf<Value>* lf = dynamic_cast<HuffmanLeaf<Value>*>( node ) ) {
                printf ("%20s: %-20d\n", lf->v.name.c_str(), dec);
            } else if ( const HuffmanBranch* in = dynamic_cast<HuffmanBranch*>( node ) ) {
                uint32_t decL = dec << 1;
                print_tree_dec( in->left, decL );

                uint32_t decR = ( dec << 1 ) + 1;
                print_tree_dec( in->right, decR );
            }
        }

    private:
        HuffmanNode* tree;
    };
} /* butterfly */

#endif /* BUTTERFLY_UTIL_HUFFMAN_HPP */
