/*
 * LLNS Copyright Start
 * Copyright (c) 2017-2020, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

#include "charMapper.h"

namespace gmlc::utilities {
CharMapper<bool> numericMapper()
{
    CharMapper<bool> mapper(false);
    mapper.addKey('0', true);
    mapper.addKey('1', true);
    mapper.addKey('2', true);
    mapper.addKey('3', true);
    mapper.addKey('4', true);
    mapper.addKey('5', true);
    mapper.addKey('6', true);
    mapper.addKey('7', true);
    mapper.addKey('8', true);
    mapper.addKey('9', true);
    mapper.addKey('+', true);
    mapper.addKey('-', true);
    mapper.addKey(' ', true);
    mapper.addKey('e', true);
    mapper.addKey('E', true);
    mapper.addKey('.', true);
    return mapper;
}

CharMapper<bool> numericStartMapper()
{
    CharMapper<bool> mapper(false);
    mapper.addKey('0', true);
    mapper.addKey('1', true);
    mapper.addKey('2', true);
    mapper.addKey('3', true);
    mapper.addKey('4', true);
    mapper.addKey('5', true);
    mapper.addKey('6', true);
    mapper.addKey('7', true);
    mapper.addKey('8', true);
    mapper.addKey('9', true);
    mapper.addKey('+', true);
    mapper.addKey('-', true);
    mapper.addKey(' ', true);
    mapper.addKey('\t', true);
    mapper.addKey('.', true);
    mapper.addKey('\n', true);
    mapper.addKey('\r', true);
    mapper.addKey('\0', true);
    return mapper;
}

CharMapper<bool> numericEndMapper()
{
    CharMapper<bool> mapper(false);
    mapper.addKey('0', true);
    mapper.addKey('1', true);
    mapper.addKey('2', true);
    mapper.addKey('3', true);
    mapper.addKey('4', true);
    mapper.addKey('5', true);
    mapper.addKey('6', true);
    mapper.addKey('7', true);
    mapper.addKey('8', true);
    mapper.addKey('9', true);
    mapper.addKey(' ', true);
    mapper.addKey('\t', true);
    mapper.addKey('\n', true);
    mapper.addKey('\r', true);
    mapper.addKey('\0', true);
    return mapper;
}

CharMapper<unsigned char> base64Mapper()
{
    CharMapper<unsigned char> b64(0xFF);
    unsigned char val{0};
    for (unsigned char indexChar = 'A'; indexChar <= 'Z'; ++indexChar) {
        b64.addKey(indexChar, val);
        ++val;
    }
    for (unsigned char indexChar = 'a'; indexChar <= 'z'; ++indexChar) {
        b64.addKey(indexChar, val);
        ++val;
    }
    for (unsigned char indexChar = '0'; indexChar <= '9'; ++indexChar) {
        b64.addKey(indexChar, val);
        ++val;
    }
    b64.addKey('+', val++);
    b64.addKey('/', val);
    return b64;
}

CharMapper<unsigned char> digitMapper()
{
    CharMapper<unsigned char> dMap(0xFF);
    unsigned char val = 0;
    for (unsigned char indexChar = '0'; indexChar <= '9'; ++indexChar) {
        dMap.addKey(indexChar, val);
        ++val;
    }
    return dMap;
}

CharMapper<unsigned char> hexMapper()
{
    CharMapper<unsigned char> dMap(0xFF);
    unsigned char val = 0;
    for (unsigned char indexChar = '0'; indexChar <= '9'; ++indexChar) {
        dMap.addKey(indexChar, val);
        ++val;
    }
    for (unsigned char indexChar = 'A'; indexChar <= 'F'; ++indexChar) {
        dMap.addKey(indexChar, val);
        ++val;
    }
    val = 10;
    for (unsigned char indexChar = 'a'; indexChar <= 'f'; ++indexChar) {
        dMap.addKey(indexChar, val);
        ++val;
    }
    return dMap;
}

CharMapper<unsigned char> pairMapper()
{
    CharMapper<unsigned char> dMap(0);
    for (unsigned char ii = 0; ii < 255; ++ii) {
        dMap.addKey(ii, ii);
    }
    dMap.addKey('(', ')');
    dMap.addKey('<', '>');
    dMap.addKey('[', ']');
    dMap.addKey('{', '}');
    dMap.addKey(')', '(');
    dMap.addKey('>', '<');
    dMap.addKey(']', '[');
    dMap.addKey('}', '{');
    dMap.addKey(0x02, 0x03);  // start of text, end of text
    dMap.addKey(0x03, 0x02);
    dMap.addKey(0x0E, 0x0F);  // XON, XOFF
    dMap.addKey(0x0F, 0x0E);
    dMap.addKey(0x11, 0x13);  // XON, XOFF
    dMap.addKey(0x13, 0x11);
    return dMap;
}

}  // namespace gmlc::utilities
