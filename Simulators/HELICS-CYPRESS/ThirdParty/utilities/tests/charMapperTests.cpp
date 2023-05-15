/*
 * LLNS Copyright Start
 * Copyright (c) 2014-2018, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

#include "gmlc/utilities/charMapper.h"

#include "gtest/gtest.h"
#include <iostream>
#include <type_traits>

using namespace gmlc::utilities;
/** test conversion to lower case*/
TEST(charMapper, numericMapper)
{
    auto v = numericMapper();
    auto dtype = std::is_same<decltype(v[1]), bool>::value;
    EXPECT_TRUE(dtype);
    EXPECT_TRUE(v['1']);
    EXPECT_TRUE(v['0']);
    EXPECT_FALSE(v['a']);
    EXPECT_FALSE(v.at('a'));
}

TEST(charMapper, numericStartMapper)
{
    auto v = numericStartMapper();
    auto dtype = std::is_same<decltype(v[1]), bool>::value;
    EXPECT_TRUE(dtype);
    EXPECT_TRUE(v['+']);
    EXPECT_FALSE(v['e']);
    EXPECT_FALSE(v['t']);
    EXPECT_TRUE(v['7']);
    EXPECT_TRUE(v['.']);
}

TEST(charMapper, numericEndMapper)
{
    auto v = numericEndMapper();
    auto dtype = std::is_same<decltype(v[1]), bool>::value;
    EXPECT_TRUE(dtype);
    EXPECT_FALSE(v['+']);
    EXPECT_FALSE(v['e']);
    EXPECT_FALSE(v['t']);
    EXPECT_TRUE(v['7']);
    EXPECT_FALSE(v['.']);
}

TEST(charMapper, base64Mapper)
{
    auto v = base64Mapper();
    auto dtype = std::is_same<decltype(v[1]), unsigned char>::value;
    EXPECT_TRUE(dtype);
    EXPECT_EQ(v.at('A'), 0);
    EXPECT_EQ(v.at('a'), 26);
    EXPECT_EQ(v.at('m'), 38);
    EXPECT_EQ(v.at('8'), 60);
    EXPECT_EQ(v.at('('), 0xFF);
}

TEST(charMapper, digitMapper)
{
    auto v = digitMapper();
    auto dtype = std::is_same<decltype(v[1]), unsigned char>::value;
    EXPECT_TRUE(dtype);
    EXPECT_EQ(v['9'], 9);
    EXPECT_EQ(v['8'], 8);
    EXPECT_EQ(v['7'], 7);
    EXPECT_EQ(v['6'], 6);
    EXPECT_EQ(v['5'], 5);
    EXPECT_EQ(v['4'], 4);
    EXPECT_EQ(v['3'], 3);
    EXPECT_EQ(v['2'], 2);
    EXPECT_EQ(v['1'], 1);
    EXPECT_EQ(v['0'], 0);
}

TEST(charMapper, hexMapper)
{
    auto v = hexMapper();
    auto dtype = std::is_same<decltype(v[1]), unsigned char>::value;
    EXPECT_TRUE(dtype);
    EXPECT_EQ(v['9'], 9);
    EXPECT_EQ(v['8'], 8);
    EXPECT_EQ(v['7'], 7);
    EXPECT_EQ(v['6'], 6);
    EXPECT_EQ(v['5'], 5);
    EXPECT_EQ(v['4'], 4);
    EXPECT_EQ(v['3'], 3);
    EXPECT_EQ(v['2'], 2);
    EXPECT_EQ(v['1'], 1);
    EXPECT_EQ(v['0'], 0);
    EXPECT_EQ(v['A'], 10);
    EXPECT_EQ(v['B'], 11);
    EXPECT_EQ(v['C'], 12);
    EXPECT_EQ(v['D'], 13);
    EXPECT_EQ(v['E'], 14);
    EXPECT_EQ(v['F'], 15);

    EXPECT_EQ(v['a'], 10);
    EXPECT_EQ(v['b'], 11);
    EXPECT_EQ(v['c'], 12);
    EXPECT_EQ(v['d'], 13);
    EXPECT_EQ(v['e'], 14);
    EXPECT_EQ(v['f'], 15);

    EXPECT_EQ(v['g'], 0xFF);
}

TEST(charMapper, pairMapper)
{
    auto v = pairMapper();
    auto dtype = std::is_same<decltype(v[1]), unsigned char>::value;
    EXPECT_TRUE(dtype);

    EXPECT_EQ(v['('], ')');
    EXPECT_EQ(v['['], ']');
    EXPECT_EQ(v['{'], '}');
    EXPECT_EQ(v['\"'], '\"');
    EXPECT_EQ(v['\''], '\'');
    EXPECT_EQ(v['`'], '`');
    EXPECT_EQ(v['<'], '>');
    EXPECT_EQ(v['>'], '<');
    EXPECT_EQ(v[')'], '(');
    EXPECT_EQ(v[']'], '[');
    EXPECT_EQ(v['}'], '{');
    EXPECT_EQ(v['|'], '|');
    EXPECT_EQ(v['a'], 'a');
}
