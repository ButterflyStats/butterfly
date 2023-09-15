/**
 * @file util_ascii_table.cpp
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

#ifndef BUTTERFLY_UTIL_ASCII_TABLE_HPP
#define BUTTERFLY_UTIL_ASCII_TABLE_HPP

#include <string>
#include <sstream>
#include <numeric>
#include <cmath>

#include <eggs/variant.hpp>

#include <butterfly/util_assert.hpp>
#include <butterfly/util_noncopyable.hpp>
#include <butterfly/util_platform.hpp>
#include "util_string.hpp"

namespace butterfly {
    namespace detail {
        /** Convert floating point into string with given precision */
        force_inline std::string precision_float( const double f, const uint32_t precision ) {
            std::stringstream s;
            s.precision( precision );
            s << f;
            return s.str();
        }

        /** Centers a string for the given row width */
        force_inline std::string align_center( std::string s, const uint32_t width ) {
            trim( s );

            std::string ret, spaces( "" );
            std::stringstream ss, sss;
            int32_t margin = ( width - s.size() );

            if ( margin > 0 ) {
                for ( int32_t i = 0; i < margin / 2; ++i ) {
                    spaces.append( " " );
                }
            }

            ret.append( spaces );
            ret.append( s );
            ret.append( spaces );

            // Append last space if margin is not a divisible by 2
            if ( ( margin % 2 ) != 0 && margin > 0 )
                ret.append( " " );

            return ret;
        }

        /** Left string alignment */
        force_inline std::string align_left( std::string s, const uint32_t width ) {
            trim( s );

            std::string ret, spaces;
            uint64_t margin = ( width - s.size() );

            if ( margin > 0 ) {
                for ( uint32_t i = 0; i < margin; ++i ) {
                    spaces.append( " " );
                }
            }

            ret.append( s );
            ret.append( spaces );
            return ret;
        }

        /** Right string alignment */
        force_inline std::string align_right( std::string s, const uint32_t width ) {
            trim( s );

            std::string ret, spaces;
            uint32_t margin = ( width - s.size() );

            if ( margin > 0 ) {
                for ( uint32_t i = 0; i < margin; ++i ) {
                    spaces.append( " " );
                }
            }

            ret.append( spaces );
            ret.append( s );
            return ret;
        }

        /** Returns the number of digits in a floating point number */
        template <typename T>
        force_inline uint32_t count_digits( T num ) {
            // Could be optimized by using d&c
            uint32_t ret = ( num < 0 ) ? 1 : 0;
            ret += log10( num ) + 1;
            return ret;
        }
    } /* detail */

    /** Simplify building ascii tables */
    class ascii_table : private noncopyable {
    public:
        /** Variant */
        template <typename... T>
        using variant = eggs::variant<T...>;
        /** Variant type used to store value */
        typedef variant<bool, int32_t, int64_t, uint32_t, uint64_t, float, std::string, const char*> variant_t;
        /** Row type */
        typedef std::vector<variant_t> row_t;

        /** Config */
        struct config_t {
            /** Is the first row the header ? */
            bool header;
            /** Print seperators */
            bool seperators;
            /** Print line number */
            bool linenumbers;
        };

        /** Constructor */
        ascii_table() : colsize( 0 ) {}

        /** Default destructor */
        ~ascii_table() = default;

        /** Adds a new row to the table */
        template <typename... T>
        void append( T&&... data ) {
            // Make sure we keep track of the maximum columns
            const uint32_t n = sizeof...( data ) + 1;
            if ( colsize < n )
                colsize = n;

            // Call internal append
            row_t row;
            append_internal( row, std::forward<T>(data)... );

            // Add row to table
            this->data.push_back( std::move( row ) );
        }

        /** Print the table */
        template <typename T>
        void print( config_t conf, T &out ) {
            if ( data.empty() )
                return;

            // @todo: Clean up this code

            // Get maximum length for each column
            std::vector<uint32_t> col_len( colsize, 0 );

            for ( auto& row : data ) {
                for ( std::size_t i = 0; i < row.size(); ++i ) {
                    uint32_t len = get_length( row[i] );
                    if ( col_len[i] < len )
                        col_len[i] = len;
                }
            }

            // Get table length
            uint32_t tbl_len = std::accumulate( col_len.begin(), col_len.end(), 0 );
            tbl_len += ( col_len.size() * 3 ) - 2;

            // Current row
            uint32_t row = 0;

            // Print header
            if ( conf.header ) {
                print_seperator( tbl_len, out );
                out << "| ";

                auto& header = data[0];
                for ( std::size_t i = 0; i < header.size(); ++i ) {
                    out << detail::align_center( variant_to_string( header[i] ), col_len[i] );

                    if ( i + 1 != header.size() )
                        out << " | ";
                }
                out << " |" << std::endl;
                ++row;
            }

            print_seperator( tbl_len, out );

            // Print data
            for ( std::size_t i = row; i < data.size(); ++i ) {
                out << "| ";

                auto& row = data[i];
                for ( std::size_t j = 0; j < row.size(); ++j ) {
                    out << detail::align_left( variant_to_string( row[j] ), col_len[j] );

                    if ( j + 1 != row.size() )
                        out << " | ";
                }

                out << " |" << std::endl;
            }

            print_seperator( tbl_len, out );
        }

    private:
        /** Data */
        std::vector<row_t> data; // vector within vector feels so dirty
        /** Maximum column size */
        uint32_t colsize;

        /** Internal append function */
        template <typename Head, typename... Tail>
        void append_internal( row_t& r, Head h, Tail&&... t ) {
            r.push_back( variant_t( h ) );
            append_internal( r, std::forward<Tail>(t)... );
        }

        template <typename Head>
        void append_internal( row_t& r, Head&& h ) {
            r.push_back( variant_t( std::move(h) ) );
        }

        /** Returns length of value */
        uint32_t get_length( variant_t& v ) {
            switch ( v.which() ) {
            // bool, [true|false]
            case 0: {
                if ( *v.target<bool>() )
                    return 4;
                else
                    return 5;
            } break;

            case 1:
                return detail::count_digits( *v.target<int32_t>() );
            case 2:
                return detail::count_digits( *v.target<int64_t>() );
            case 3:
                return detail::count_digits( *v.target<uint32_t>() );
            case 4:
                return detail::count_digits( *v.target<uint64_t>() );
            case 5:
                return detail::precision_float( *v.target<float>(), 7 ).size();
            case 6:
                return v.target<std::string>()->size();
            case 7:
                return strlen( *v.target<const char*>() );
            }

            return 5;
        }

        /** Converts variant to string */
        std::string variant_to_string( variant_t& v ) {
            switch ( v.which() ) {
            // bool, [true|false]
            case 0: {
                if ( *v.target<bool>() )
                    return "true";
                else
                    return "false";
            } break;

            case 1:
                return std::to_string( *v.target<int32_t>() );
            case 2:
                return std::to_string( *v.target<int64_t>() );
            case 3:
                return std::to_string( *v.target<uint32_t>() );
            case 4:
                return std::to_string( *v.target<uint64_t>() );
            case 5:
                return detail::precision_float( *v.target<float>(), 7 );
            case 6:
                return *v.target<std::string>();
            case 7:
                return std::string( *v.target<const char*>() );
            }

            return "Error";
        }

        /** Prints a seperator line */
        template <typename T>
        void print_seperator( uint32_t len, T &out ) {
            ASSERT_GREATER_EQ( len, 2, "Table too small" );

            out << "+";
            for ( uint32_t i = 0; i < ( len - 2 ); ++i ) {
                out << "-";
            }
            out << "+" << std::endl;;
        }
    };
} /* butterfly */

#endif /* BUTTERFLY_UTIL_ASCII_TABLE_HPP */
