/*
Copyright (c) 2017-2020,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "gmlc/utilities/TimeSeriesMulti.hpp"

#include "gtest/gtest.h"
#include <iostream>

using namespace gmlc::utilities;

namespace gmlc::utilities {
template class TimeSeriesMulti<double, double>;
}  // namespace gmlc::utilities
