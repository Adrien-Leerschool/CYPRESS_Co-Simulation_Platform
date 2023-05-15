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

/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

/** changelog
June 2022 changed to support std::string_view where applicable
*/

#include "string_viewOps.h"

#include "generic_string_ops.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <string_view>

namespace gmlc::utilities::string_viewOps {

void trimString(std::string_view& input, std::string_view trimCharacters)
{
    input.remove_suffix(
        input.length() -
        std::min(input.find_last_not_of(trimCharacters) + 1, input.size()));
    input.remove_prefix(
        std::min(input.find_first_not_of(trimCharacters), input.size()));
}

std::string_view trim(std::string_view input, std::string_view trimCharacters)
{
    const auto strStart = input.find_first_not_of(trimCharacters);
    if (strStart == std::string::npos) {
        return {};  // no content
    }

    const auto strEnd = input.find_last_not_of(trimCharacters);

    return input.substr(strStart, strEnd - strStart + 1);
}

void trim(string_viewVector& input, std::string_view trimCharacters)
{
    for (auto& istr : input) {
        istr = trim(istr, trimCharacters);
    }
}

std::string_view
    getTailString(std::string_view input, char separationCharacter) noexcept
{
    auto sepLoc = input.find_last_of(separationCharacter);
    if (sepLoc != std::string_view::npos) {
        input.remove_prefix(sepLoc + 1);
    }
    return input;
}

std::string_view
    getTailString(std::string_view input, std::string_view sep) noexcept
{
    auto sepLoc = input.rfind(sep);
    if (sepLoc != std::string_view::npos) {
        input.remove_prefix(sepLoc + sep.size());
    }
    return input;
}

std::string_view getTailString_any(
    std::string_view input,
    std::string_view separationCharacters) noexcept
{
    auto sepLoc = input.find_last_of(separationCharacters);
    if (sepLoc != std::string_view::npos) {
        input.remove_prefix(sepLoc + 1);
    }
    return input;
}

std::string_view removeQuotes(std::string_view str)
{
    std::string_view ret = trim(str);
    if (!ret.empty()) {
        if ((ret.front() == '\"') || (ret.front() == '\'') ||
            (ret.front() == '`')) {
            if (ret.back() == ret.front()) {
                return ret.substr(1, ret.size() - 2);
            }
        }
    }
    return ret;
}

static const auto pmap = pairMapper();

std::string_view removeBrackets(std::string_view str)
{
    std::string_view ret = trim(str);
    if (!ret.empty()) {
        if ((ret.front() == '[') || (ret.front() == '(') ||
            (ret.front() == '{') || (ret.front() == '<')) {
            if (static_cast<unsigned char>(ret.back()) == pmap[ret.front()]) {
                return ret.substr(1, ret.size() - 2);
            }
        }
    }
    return ret;
}

std::string_view merge(std::string_view string1, std::string_view string2)
{
    ptrdiff_t diff = (string2.data() - string1.data()) -
        static_cast<ptrdiff_t>(string1.length());
    if ((diff >= 0) &&
        (diff < 24))  // maximum of 23 characters between the strings
    {
        return {string1.data(), diff + string1.length() + string2.length()};
    }
    if (string1.empty()) {
        return string2;
    }
    if (string2.empty()) {
        return string1;
    }
    throw(std::out_of_range("unable to merge string_views"));
}

string_viewVector split(
    std::string_view str,
    std::string_view delimiters,
    delimiter_compression compression)
{
    return generalized_string_split(
        str, delimiters, (compression == delimiter_compression::on));
}

string_viewVector splitlineQuotes(
    std::string_view line,
    std::string_view delimiters,
    std::string_view quoteChars,
    delimiter_compression compression)
{
    bool compress = (compression == delimiter_compression::on);
    return generalized_section_splitting(
        line, delimiters, quoteChars, pmap, compress);
}

string_viewVector splitlineBracket(
    std::string_view line,
    std::string_view delimiters,
    std::string_view bracketChars,
    delimiter_compression compression)
{
    bool compress = (compression == delimiter_compression::on);
    return generalized_section_splitting(
        line, delimiters, bracketChars, pmap, compress);
}

int toIntSimple(std::string_view input)
{
    int ret = 0;
    for (auto testChar : input) {
        if (testChar >= '0' && testChar <= '9') {
            ret = 10 * ret + (testChar - '0');
        }
    }
    return ret;
}

static constexpr std::string_view digits("0123456789");
int trailingStringInt(
    std::string_view input,
    std::string_view& output,
    int defNum)
{
    if (input.empty() || (input.back() < '0' || input.back() > '9')) {
        output = input;
        return defNum;
    }
    int num = defNum;
    auto pos1 = input.find_last_not_of(digits);
    if (pos1 == std::string_view::npos)  // in case the whole thing is a number
    {
        if (input.length() <= 10) {
            output = std::string_view{};
            num = toIntSimple(input);
            return num;
        }
        pos1 = input.length() - 10;
    }
    size_t length = input.length();
    if (pos1 == length - 2) {
        num = input.back() - '0';
    } else if (length <= 10 || pos1 >= length - 10) {
        num = toIntSimple(input.substr(pos1 + 1));
    } else {
        num = toIntSimple(input.substr(length - 9));
        pos1 = length - 10;
    }

    if (input[pos1] == '_' || input[pos1] == '#') {
        output = input.substr(0, pos1);
    } else {
        output = input.substr(0, pos1 + 1);
    }

    return num;
}

int trailingStringInt(std::string_view input, int defNum)
{
    if (input.empty() || (input.back() < '0' || input.back() > '9')) {
        return defNum;
    }

    auto pos1 = input.find_last_not_of(digits);
    if (pos1 == std::string_view::npos)  // in case the whole thing is a number
    {
        if (input.length() <= 10) {
            return toIntSimple(input);
        }
        pos1 = input.length() - 10;
    }
    size_t length = input.length();
    if (pos1 == length - 2) {
        return input.back() - '0';
    }
    if ((length <= 10) || (pos1 >= length - 10)) {
        return toIntSimple(input.substr(pos1 + 1));
    }
    return toIntSimple(input.substr(length - 9));
}

}  // namespace gmlc::utilities::string_viewOps
