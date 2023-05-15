/*
   base64.cpp and base64.h
   Copyright (C) 2004-2008 René Nyffenegger
   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.
   3. This notice may not be removed or altered from any source distribution.
   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

   modified by Philip Top LLNL 2017:  added CharMapper objects instead of find
   and C++ based overloads
   Modified by Philip Top LLNL 2019: to be in a namespace and further comments
   and test cases

   Modified by Philip Top LLNL 2022: to use std::string_view
*/

#include "base64.h"
#include "charMapper.h"
#include <string_view>

#include <array>
#include <iostream>
#include <vector>

namespace gmlc::utilities {
static constexpr std::string_view base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                 "abcdefghijklmnopqrstuvwxyz"
                                                 "0123456789+/";

static const CharMapper<unsigned char> b64Map = base64Mapper();

static inline bool isBase64(unsigned char testChar)
{
    return (b64Map[testChar] < 0xffU);
}
std::string base64_encode(void const* bytes_to_encode, size_t in_len)
{
    const auto* b2e = static_cast<const unsigned char*>(bytes_to_encode);
    std::string ret;
    ret.reserve((in_len * 4) / 3 + 2);
    int index{0};
    std::array<unsigned char, 3> char_array_3{{0U, 0U, 0U}};
    std::array<unsigned char, 4> char_array_4{{0U, 0U, 0U, 0U}};

    while (in_len-- != 0) {
        char_array_3[index++] = *b2e++;
        if (index == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfcU) >> 2U;
            char_array_4[1] = ((char_array_3[0] & 0x03U) << 4U) +
                ((char_array_3[1] & 0xf0U) >> 4U);
            char_array_4[2] = ((char_array_3[1] & 0x0fU) << 2U) +
                ((char_array_3[2] & 0xc0U) >> 6U);
            char_array_4[3] = char_array_3[2] & 0x3fU;

            for (index = 0; (index < 4); index++) {
                ret.push_back(base64_chars[char_array_4[index]]);
            }
            index = 0;
        }
    }

    if (index != 0) {
        for (int jj = index; jj < 3; ++jj) {
            char_array_3[jj] = '\0';
        }

        char_array_4[0] = (char_array_3[0] & 0xfcU) >> 2U;
        char_array_4[1] = ((char_array_3[0] & 0x03U) << 4U) +
            ((char_array_3[1] & 0xf0U) >> 4U);
        char_array_4[2] = ((char_array_3[1] & 0x0fU) << 2U) +
            ((char_array_3[2] & 0xc0U) >> 6U);
        char_array_4[3] = char_array_3[2] & 0x3fU;

        for (int jj = 0; (jj < index + 1); ++jj) {
            ret.push_back(base64_chars[char_array_4[jj]]);
        }

        while ((index++ < 3)) {
            ret.push_back('=');
        }
    }

    return ret;
}

std::vector<unsigned char>
    base64_decode(std::string_view encoded_string, size_t offset)
{
    auto in_len = encoded_string.size() - offset - 1;
    int indexOut = 0;
    int indexIn = static_cast<int>(offset);
    std::array<unsigned char, 3> char_array_3{{0U, 0U, 0U}};
    std::array<unsigned char, 4> char_array_4{{0U, 0U, 0U, 0U}};
    std::vector<unsigned char> ret;
    ret.reserve((in_len * 4) / 3 + 2);

    while (((in_len--) != 0U) && (encoded_string[indexIn] != '=') &&
           isBase64(encoded_string[indexIn])) {
        char_array_4[indexOut++] = encoded_string[indexIn];
        indexIn++;
        if (indexOut == 4) {
            char_array_4[0] = b64Map[char_array_4[0]];
            char_array_4[1] = b64Map[char_array_4[1]];
            char_array_4[2] = b64Map[char_array_4[2]];
            char_array_4[3] = b64Map[char_array_4[3]];

            char_array_3[0] =
                (char_array_4[0] << 2U) + ((char_array_4[1] & 0x30U) >> 4U);
            char_array_3[1] = ((char_array_4[1] & 0xfU) << 4U) +
                ((char_array_4[2] & 0x3cU) >> 2U);
            char_array_3[2] =
                ((char_array_4[2] & 0x3U) << 6U) + char_array_4[3];

            ret.push_back(char_array_3[0]);
            ret.push_back(char_array_3[1]);
            ret.push_back(char_array_3[2]);

            indexOut = 0;
        }
    }

    if (indexOut > 0) {
        for (int j = indexOut; j < 4; j++) {
            char_array_4[j] = 0;
        }
        char_array_4[0] = b64Map[char_array_4[0]];
        char_array_4[1] = b64Map[char_array_4[1]];
        char_array_4[2] = b64Map[char_array_4[2]];
        char_array_4[3] = b64Map[char_array_4[3]];

        char_array_3[0] =
            (char_array_4[0] << 2U) + ((char_array_4[1] & 0x30U) >> 4U);
        char_array_3[1] = ((char_array_4[1] & 0xfU) << 4U) +
            ((char_array_4[2] & 0x3cU) >> 2U);
        char_array_3[2] = ((char_array_4[2] & 0x3U) << 6U) + char_array_4[3];

        for (int j = 0; (j < indexOut - 1); j++) {
            ret.push_back(char_array_3[j]);
        }
    }

    return ret;
}

std::string
    base64_decode_to_string(std::string_view encoded_string, size_t offset)
{
    auto in_len = encoded_string.size();
    int indexOut{0};
    int indexIn{static_cast<int>(offset)};
    std::array<unsigned char, 3> char_array_3{{0U, 0U, 0U}};
    std::array<unsigned char, 4> char_array_4{{0U, 0U, 0U, 0U}};
    std::string ret;
    ret.reserve(in_len);

    while (((in_len--) != 0U) && (encoded_string[indexIn] != '=') &&
           isBase64(encoded_string[indexIn])) {
        char_array_4[indexOut++] = encoded_string[indexIn];
        indexIn++;
        if (indexOut == 4) {
            char_array_4[0] = b64Map[char_array_4[0]];
            char_array_4[1] = b64Map[char_array_4[1]];
            char_array_4[2] = b64Map[char_array_4[2]];
            char_array_4[3] = b64Map[char_array_4[3]];

            char_array_3[0] =
                (char_array_4[0] << 2U) + ((char_array_4[1] & 0x30U) >> 4U);
            char_array_3[1] = ((char_array_4[1] & 0xfU) << 4U) +
                ((char_array_4[2] & 0x3cU) >> 2U);
            char_array_3[2] =
                ((char_array_4[2] & 0x3U) << 6U) + char_array_4[3];

            ret.push_back(char_array_3[0]);
            ret.push_back(char_array_3[1]);
            ret.push_back(char_array_3[2]);

            indexOut = 0;
        }
    }

    if (indexOut > 0) {
        for (int j = indexOut; j < 4; j++) {
            char_array_4[j] = 0;
        }
        char_array_4[0] = b64Map[char_array_4[0]];
        char_array_4[1] = b64Map[char_array_4[1]];
        char_array_4[2] = b64Map[char_array_4[2]];
        char_array_4[3] = b64Map[char_array_4[3]];

        char_array_3[0] =
            (char_array_4[0] << 2U) + ((char_array_4[1] & 0x30U) >> 4U);
        char_array_3[1] = ((char_array_4[1] & 0xfU) << 4U) +
            ((char_array_4[2] & 0x3cU) >> 2U);
        char_array_3[2] = ((char_array_4[2] & 0x3U) << 6U) + char_array_4[3];

        for (int j = 0; (j < indexOut - 1); j++) {
            ret.push_back(char_array_3[j]);
        }
    }

    return ret;
}

/** decode a string to the specified memory location*/
size_t
    base64_decode(std::string_view encoded_string, void* data, size_t max_size)
{
    auto in_len = encoded_string.size();
    int indexOut = 0;
    int indexIn = 0;
    std::array<unsigned char, 3> char_array_3{{'\0', '\0', '\0'}};
    std::array<unsigned char, 4> char_array_4{{'\0', '\0', '\0', '\0'}};
    auto* outData = static_cast<unsigned char*>(data);
    size_t dataIndex = 0U;
    while (((in_len--) != 0U) && (encoded_string[indexIn] != '=') &&
           isBase64(encoded_string[indexIn])) {
        char_array_4[indexOut++] = encoded_string[indexIn];
        indexIn++;
        if (indexOut == 4) {
            char_array_4[0] = b64Map[char_array_4[0]];
            char_array_4[1] = b64Map[char_array_4[1]];
            char_array_4[2] = b64Map[char_array_4[2]];
            char_array_4[3] = b64Map[char_array_4[3]];

            char_array_3[0] =
                (char_array_4[0] << 2U) + ((char_array_4[1] & 0x30U) >> 4U);
            char_array_3[1] = ((char_array_4[1] & 0xfU) << 4U) +
                ((char_array_4[2] & 0x3cU) >> 2U);
            char_array_3[2] =
                ((char_array_4[2] & 0x3U) << 6U) + char_array_4[3];
            if (dataIndex + 2 < max_size) {
                outData[dataIndex++] = char_array_3[0];
                outData[dataIndex++] = char_array_3[1];
                outData[dataIndex++] = char_array_3[2];
            } else if (dataIndex + 1 < max_size) {
                outData[dataIndex++] = char_array_3[0];
                outData[dataIndex++] = char_array_3[1];
                return dataIndex;
            } else if (dataIndex < max_size) {
                outData[dataIndex++] = char_array_3[0];
                return dataIndex;
            }

            indexOut = 0;
        }
    }

    if (indexOut > 0) {
        for (int j = indexOut; j < 4; j++) {
            char_array_4[j] = 0;
        }
        char_array_4[0] = b64Map[char_array_4[0]];
        char_array_4[1] = b64Map[char_array_4[1]];
        char_array_4[2] = b64Map[char_array_4[2]];
        char_array_4[3] = b64Map[char_array_4[3]];

        char_array_3[0] =
            (char_array_4[0] << 2U) + ((char_array_4[1] & 0x30U) >> 4U);
        char_array_3[1] = ((char_array_4[1] & 0xfU) << 4U) +
            ((char_array_4[2] & 0x3cU) >> 2U);
        char_array_3[2] = ((char_array_4[2] & 0x3U) << 6U) + char_array_4[3];

        for (int j = 0; (j < indexOut - 1); j++) {
            if (dataIndex < max_size) {
                outData[dataIndex++] = char_array_3[j];
            }
        }
    }
    return dataIndex;
}
}  // namespace gmlc::utilities
