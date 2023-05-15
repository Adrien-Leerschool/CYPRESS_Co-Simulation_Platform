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

#include "vectorOps.hpp"

namespace gmlc::utilities {
double solve2x2(
    double v11,
    double v12,
    double v21,
    double v22,
    double result1,
    double result2,
    double& value1,
    double& value2)
{
    double det = 1.0 / (v11 * v22 - v12 * v21);
    value1 = det * (v22 * result1 - v12 * result2);
    value2 = det * (-v21 * result1 + v11 * result2);
    return det;
}

std::array<double, 3> solve3x3(
    const std::array<std::array<double, 3>, 3>& input,
    const std::array<double, 3>& vals)
{
    double a11 = input[0][0];
    double a12 = input[0][1];
    double a13 = input[0][2];
    double a21 = input[1][0];
    double a22 = input[1][1];
    double a23 = input[1][2];
    double a31 = input[2][0];
    double a32 = input[2][1];
    double a33 = input[2][2];

    double i11 = a33 * a22 - a32 * a23;
    double i12 = -(a33 * a12 - a32 * a13);
    double i13 = a23 * a12 - a22 * a13;

    double i21 = -(a33 * a21 - a31 * a23);
    double i22 = a33 * a11 - a31 * a13;
    double i23 = -(a23 * a11 - a21 * a13);

    double i31 = a32 * a21 - a31 * a22;
    double i32 = -(a32 * a11 - a31 * a12);
    double i33 = a22 * a11 - a21 * a12;

    double deti = 1.0 / (a11 * i11 + a21 * i12 + a31 * i13);

    std::array<double, 3> output{
        {deti * (i11 * vals[0] + i12 * vals[1] + i13 * vals[2]),
         deti * (i21 * vals[0] + i22 * vals[1] + i23 * vals[2]),
         deti * (i31 * vals[0] + i32 * vals[1] + i33 * vals[2])}};

    return output;
}

// Linear Interpolation function
std::vector<double> interpolateLinear(
    const std::vector<double>& timeIn,
    const std::vector<double>& valIn,
    const std::vector<double>& timeOut)
{
    size_t minSize = (std::min)(valIn.size(), timeIn.size());
    std::vector<double> out(timeOut.size(), 0);
    size_t indexOut = 0;
    size_t indexIn = 0;
    while (timeOut[indexOut] <= timeIn[0]) {
        out[indexOut] = valIn[0] -
            (valIn[1] - valIn[0]) / (timeIn[1] - timeIn[0]) *
                (timeIn[0] - timeOut[indexOut]);
        ++indexOut;
    }
    while (indexOut < timeOut.size()) {
        while (timeIn[indexIn + 1] < timeOut[indexOut]) {
            ++indexIn;
            if (indexIn + 1 == minSize) {
                goto breakLoop;  // break out of a double loop
            }
        }
        out[indexOut] = valIn[indexIn] +
            (valIn[indexIn + 1] - valIn[indexIn]) /
                (timeIn[indexIn + 1] - timeIn[indexIn]) *
                (timeOut[indexOut] - timeIn[indexIn]);
        // out[jj] = std::fma((valIn[kk + 1] - valIn[kk]) / (timeIn[kk + 1]
        // - timeIn[kk]), (timeOut[jj] - timeIn[kk]), valIn[kk]);
        ++indexOut;
    }
breakLoop:
    while (indexOut < timeOut.size()) {
        out[indexOut] = valIn[minSize - 1] +
            (valIn[minSize - 1] - valIn[minSize - 2]) /
                (timeIn[minSize - 1] - timeIn[minSize - 2]) *
                (timeOut[indexOut] - timeIn[minSize - 1]);
        ++indexOut;
    }
    return out;
}
}  // namespace gmlc::utilities
