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

#include "gmlc/utilities/stringConversion.h"

#include "gtest/gtest.h"
#include <iostream>
#include <type_traits>

using namespace gmlc::utilities;
/** test conversion to lower case*/
TEST(stringconversion, simple_integer_conversions_test)
{
    auto a = numeric_conversion<int>("457", -1);
    EXPECT_EQ(a, 457);
    auto b = numeric_conversion<int64_t>("-457", -1);
    EXPECT_EQ(b, -457);
    static_assert(
        std::is_same<decltype(b), int64_t>::value,
        "conversion types do not match");
    auto c = numeric_conversion<unsigned char>("25", 0xFF);
    EXPECT_EQ(c, 25);
    auto d = numeric_conversion<int16_t>("-7629", 0xFF);
    EXPECT_EQ(d, -7629);
    EXPECT_EQ(sizeof(d), 2U);

    auto e = numeric_conversion<unsigned int>("-1", 0);
    EXPECT_EQ(e, static_cast<unsigned int>(-1));

    auto f = numeric_conversion("FF3q", -234);
    EXPECT_EQ(f, -234);

    auto g = numeric_conversion<uint32_t>("978", 0);
    EXPECT_EQ(g, 978UL);

    auto h = numeric_conversion<uint64_t>("123456789123456789", 0);
    EXPECT_EQ(h, 123456789123456789ULL);

    auto i = numeric_conversion<int>("-Bad", -35);
    EXPECT_EQ(i, -35);
}

TEST(stringconversion, simple_floating_point_conversions_test)
{
    const double closeDef = 0.0000000001;
    auto a = numeric_conversion<float>("457", -1);
    EXPECT_EQ(a, 457);
    auto b = numeric_conversion<double>("234.123131", -1);
    EXPECT_NEAR(b, 234.123131, closeDef);
    static_assert(
        std::is_same<decltype(b), double>::value,
        "conversion types do not match");
    auto c = numeric_conversion<double>(".456", 0xFF);
    EXPECT_NEAR(c, .456, closeDef);
    auto d = numeric_conversion<long double>("45.456e27", 0xFF);
    EXPECT_NEAR(static_cast<double>(d), 45.456e27, closeDef);
    EXPECT_EQ(sizeof(d), sizeof(long double));

    auto e = numeric_conversion<double>("-456.234", 0);
    EXPECT_NEAR(e, -456.234, closeDef);

    auto f = numeric_conversion<double>("-23E-2", 0);
    EXPECT_NEAR(f, -0.23, closeDef);

    auto g = numeric_conversion("FF3q", 45.34);
    EXPECT_NEAR(g, 45.34, closeDef);
}

TEST(stringconversion, simple_integer_conversion_complete_test)
{
    auto a = numeric_conversionComplete<int>("457  ", -1);
    EXPECT_EQ(a, 457);
    auto b = numeric_conversionComplete<int64_t>("-457", -1);
    EXPECT_EQ(b, -457);
    static_assert(
        std::is_same<decltype(b), int64_t>::value,
        "conversion types do not match");
    auto c = numeric_conversionComplete<unsigned char>("25", 0xFF);
    EXPECT_EQ(c, 25);
    auto d = numeric_conversionComplete<int16_t>("-7629", 0xFF);
    EXPECT_EQ(d, -7629);
    EXPECT_EQ(sizeof(d), 2U);

    auto e = numeric_conversionComplete<unsigned int>("-1", 0);
    EXPECT_EQ(e, static_cast<unsigned int>(-1));

    auto f = numeric_conversionComplete("FF3q", -234);
    EXPECT_EQ(f, -234);

    auto g = numeric_conversionComplete<uint32_t>("978F9", 0);
    EXPECT_EQ(g, 0UL);

    auto h = numeric_conversionComplete<uint64_t>(" 123456789123456789 ", 0);
    EXPECT_EQ(h, 123456789123456789ULL);

    auto i = numeric_conversionComplete<int>("-Bad", -35);
    EXPECT_EQ(i, -35);

    auto j = numeric_conversionComplete<int>("000987", -35);
    EXPECT_EQ(j, 987);
}

TEST(stringconversion, str2_vector)
{
    auto v = str2vector<int>("1,2,3,4", 0);

    std::vector<int> v2{1, 2, 3, 4};
    EXPECT_EQ(v, v2);

    v = str2vector<int>("1,2,N,4", 0);

    std::vector<int> v3{1, 2, 0, 4};
    EXPECT_EQ(v, v3);

    v = str2vector<int>("1:2:-N:4", 0, ":");

    EXPECT_EQ(v, v3);

    v = str2vector<int>("1:2:-N|4", 0, ":|");

    EXPECT_EQ(v, v3);
}

TEST(stringconversion, str2_vectorb)
{
    stringVector input{"1", "2", "3", "4"};
    auto v = str2vector<int>(input, 0);

    std::vector<int> v2{1, 2, 3, 4};
    EXPECT_EQ(v, v2);

    input[2] = "-N";
    v = str2vector<int>(input, 0);

    std::vector<int> v3{1, 2, 0, 4};
    EXPECT_EQ(v, v3);

    input[0] = "  1   ";
    v = str2vector<int>(input, 0);

    EXPECT_EQ(v, v3);
}

TEST(stringconversion, simple_floating_point_conversionsComplete_test)
{
    const double closeDef = 0.0000000001;
    auto a = numeric_conversionComplete<float>("457", -1);
    EXPECT_EQ(a, 457);
    auto b = numeric_conversionComplete<double>("234.123131", -1);
    EXPECT_NEAR(b, 234.123131, closeDef);
    static_assert(
        std::is_same<decltype(b), double>::value,
        "conversion types do not match");
    auto c = numeric_conversionComplete<double>(".456", 0xFF);
    EXPECT_NEAR(c, .456, closeDef);
    auto d = numeric_conversionComplete<long double>("45.456e27", 0xFF);
    EXPECT_NEAR(static_cast<double>(d), 45.456e27, closeDef);
    EXPECT_EQ(sizeof(d), sizeof(long double));

    auto e = numeric_conversionComplete<double>("-456.234g", 0);
    EXPECT_NEAR(e, 0.0, closeDef);

    auto f = numeric_conversionComplete<double>("-23E-2", 0);
    EXPECT_NEAR(f, -0.23, closeDef);

    auto g = numeric_conversionComplete("FF3q", 45.34);
    EXPECT_NEAR(g, 45.34, closeDef);

    auto h = numeric_conversionComplete("-FF3q45", 18.7);
    EXPECT_NEAR(h, 18.7, closeDef);
}
