/**
 * @file base64.hpp
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
 *
 * @par Description
 *    Base64 encoder.
 */

 #ifndef BUTTERFLYJS_BASE64_HPP
 #define BUTTERFLYJS_BASE64_HPP

#include <string>

namespace butterfly {
    static const std::string base64_chars =
         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
         "abcdefghijklmnopqrstuvwxyz"
         "0123456789+/";

    std::string base64_encode(std::string str) {
        std::string ret;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];
        unsigned char* bytes_to_encode = (unsigned char*)str.c_str();
        unsigned int in_len = str.size();

        while (in_len--) {
            char_array_3[i++] = *(bytes_to_encode++);

            if (i == 3) {
                char_array_4[0] =  (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] =   char_array_3[2] & 0x3f;

                for( i = 0; i < 4 ; i++ ) {
                    ret += base64_chars[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i) {
            for(j = i; j < 3; j++) {
                char_array_3[j] = '\0';
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++) {
                ret += base64_chars[char_array_4[j]];
            }

            while((i++ < 3)) {
              ret += '=';
          }
        }

        return ret;
    }
}

#endif /* BUTTERFLYJS_BASE64_HPP */
