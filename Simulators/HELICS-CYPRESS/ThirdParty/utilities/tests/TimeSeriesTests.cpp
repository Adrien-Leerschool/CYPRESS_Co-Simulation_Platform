/*
Copyright (c) 2017-2020,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "gmlc/utilities/TimeSeries.hpp"

#include "gtest/gtest.h"
#include <iostream>

using namespace gmlc::utilities;

namespace gmlc::utilities {
template class TimeSeries<double, double>;
}  // namespace gmlc::utilities

TEST(timeSeries, basic_test)
{
    TimeSeries<> simple;

    simple.addData(0, 5.0);
    simple.addData(1, 6.0);

    EXPECT_EQ(simple.size(), 2U);
    EXPECT_FALSE(simple.empty());
    EXPECT_EQ(simple.data(0), 5.0);
    EXPECT_EQ(simple.time(0), 0.0);
    EXPECT_EQ(simple.data(1), 6.0);
    EXPECT_EQ(simple.time(1), 1.0);

    simple.clear();

    EXPECT_TRUE(simple.empty());
}

TEST(timeSeries, basic_test_vector_input)
{
    TimeSeries<> simple;

    simple.addData({0.0, 1.0}, {5.0, 6.0});
    EXPECT_EQ(simple.size(), 2U);
    EXPECT_FALSE(simple.empty());
    EXPECT_EQ(simple.data(0), 5.0);
    EXPECT_EQ(simple.time(0), 0.0);
    EXPECT_EQ(simple.data(1), 6.0);
    EXPECT_EQ(simple.time(1), 1.0);

    simple.clear();

    EXPECT_TRUE(simple.empty());

    simple.addData({0.0, 1.0}, {5.0, 6.0});

    simple.addData({2.0, 3.0, 4.0}, {5.0, 6.0, 7.0});
    EXPECT_EQ(simple.size(), 5U);
    simple.addData(5.0, 8.0);
    EXPECT_EQ(simple.size(), 6U);
}

TEST(timeSeries, last_data)
{
    TimeSeries<int, double> ts;

    EXPECT_TRUE(ts.empty());

    ts.addData({0.0, 1.0}, {5, 6});

    EXPECT_EQ(ts.lastData(), 6);
}
