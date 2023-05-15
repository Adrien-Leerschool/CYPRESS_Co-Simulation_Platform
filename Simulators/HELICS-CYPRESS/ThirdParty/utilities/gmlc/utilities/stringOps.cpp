/*
 * LLNS Copyright Start
 * Copyright (c) 2017, Lawrence Livermore National Security
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
June 2022 changed to support string view where applicable
*/
#include "stringOps.h"

#include "generic_string_ops.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <random>
#include <utility>

namespace gmlc::utilities {
static auto lower = [](char charToMakerLowerCase) -> char {
    return (charToMakerLowerCase >= 'A' && charToMakerLowerCase <= 'Z') ?
        (charToMakerLowerCase - ('Z' - 'z')) :
        charToMakerLowerCase;
};

static auto upper = [](char charToMakeUpperCase) -> char {
    return (charToMakeUpperCase >= 'a' && charToMakeUpperCase <= 'z') ?
        (charToMakeUpperCase + ('Z' - 'z')) :
        charToMakeUpperCase;
};

std::string convertToLowerCase(std::string_view input)
{
    std::string out(input);
    std::transform(out.begin(), out.end(), out.begin(), lower);
    return out;
}

std::string convertToUpperCase(std::string_view input)
{
    std::string out(input);
    std::transform(out.begin(), out.end(), out.begin(), upper);
    return out;
}

void makeLowerCase(std::string& input)
{
    std::transform(input.begin(), input.end(), input.begin(), lower);
}

void makeUpperCase(std::string& input)
{
    std::transform(input.begin(), input.end(), input.begin(), upper);
}

namespace stringOps {
    stringVector splitline(
        std::string_view line,
        std::string_view delimiters,
        delimiter_compression compression)
    {
        return generalized_string_split<std::string_view, std::string>(
            line, delimiters, (compression == delimiter_compression::on));
    }

    stringVector splitline(std::string_view line, char del)
    {
        return generalized_string_split<std::string_view, std::string>(
            line, std::string_view{&del, 1}, false);
    }

    void splitline(
        std::string_view line,
        stringVector& strVec,
        std::string_view delimiters,
        delimiter_compression compression)
    {
        strVec = generalized_string_split<std::string_view, std::string>(
            line, delimiters, (compression == delimiter_compression::on));
    }

    void splitline(std::string_view line, stringVector& strVec, char del)
    {
        strVec = generalized_string_split<std::string_view, std::string>(
            line, std::string_view{&del, 1}, false);
    }

    static const auto pmap = pairMapper();

    stringVector splitlineQuotes(
        std::string_view line,
        std::string_view delimiters,
        std::string_view quoteChars,
        delimiter_compression compression)
    {
        bool compress = (compression == delimiter_compression::on);
        return generalized_section_splitting<std::string_view, std::string>(
            line, delimiters, quoteChars, pmap, compress);
    }

    stringVector splitlineBracket(
        std::string_view line,
        std::string_view delimiters,
        std::string_view bracketChars,
        delimiter_compression compression)
    {
        bool compress = (compression == delimiter_compression::on);
        return generalized_section_splitting<std::string_view, std::string>(
            line, delimiters, bracketChars, pmap, compress);
    }

    void trimString(std::string& input, std::string_view whitespace)
    {
        input.erase(input.find_last_not_of(whitespace) + 1);
        input.erase(0, input.find_first_not_of(whitespace));
    }

    std::string trim(std::string_view input, std::string_view whitespace)
    {
        const auto strStart = input.find_first_not_of(whitespace);
        if (strStart == std::string::npos) {
            return {};  // no content
        }

        const auto strEnd = input.find_last_not_of(whitespace);

        return std::string{input.substr(strStart, strEnd - strStart + 1)};
    }

    void trim(stringVector& input, std::string_view whitespace)
    {
        for (auto& str : input) {
            trimString(str, whitespace);
        }
    }

    static constexpr std::string_view digits("0123456789");
    int trailingStringInt(
        std::string_view input,
        std::string& output,
        int defNum) noexcept
    {
        if ((input.empty()) || (isdigit(input.back()) == 0)) {
            output = input;
            return defNum;
        }
        int num = defNum;
        auto pos1 = input.find_last_not_of(digits);
        if (pos1 == std::string::npos)  // in case the whole thing is a number
        {
            if (input.length() <= 10) {
                output.clear();
                std::from_chars(input.data(), input.data() + input.size(), num);
                return num;
            }
            pos1 = input.length() - 10;
        }

        size_t length = input.length();
        if (pos1 == length - 2) {
            num = input.back() - '0';
        } else if (length <= 10 || pos1 >= length - 10) {
            auto sub = input.substr(pos1 + 1);
            std::from_chars(sub.data(), sub.data() + sub.size(), num);
        } else {
            auto sub = input.substr(length - 9);
            std::from_chars(sub.data(), sub.data() + sub.size(), num);
            pos1 = length - 10;
        }

        if (input[pos1] == '_' || input[pos1] == '#') {
            output = input.substr(0, pos1);
        } else {
            output = input.substr(0, pos1 + 1);
        }

        return num;
    }

    int trailingStringInt(std::string_view input, int defNum) noexcept
    {
        if ((input.empty()) || (isdigit(input.back()) == 0)) {
            return defNum;
        }

        auto pos1 = input.find_last_not_of(digits);
        int num{0};
        if (pos1 == std::string::npos)  // in case the whole thing is a number
        {
            if (input.length() <= 10) {
                std::from_chars(input.data(), input.data() + input.size(), num);
                return num;
            }
            pos1 = input.length() - 10;
        }

        size_t length = input.length();
        if (pos1 == length - 2) {
            return input.back() - '0';
        }
        if ((length <= 10) || (pos1 >= length - 10)) {
            auto sub = input.substr(pos1 + 1);

            std::from_chars(sub.data(), sub.data() + sub.size(), num);
            return num;
        }
        auto sub = input.substr(length - 9);

        std::from_chars(sub.data(), sub.data() + sub.size(), num);
        return num;
    }

    std::string removeQuotes(std::string_view str)
    {
        auto newString = trim(str);
        if (!newString.empty()) {
            if ((newString.front() == '\"') || (newString.front() == '\'') ||
                (newString.front() == '`')) {
                if (newString.back() == newString.front()) {
                    newString.pop_back();
                    newString.erase(0, 1);
                }
            }
        }
        return newString;
    }

    std::string removeBrackets(std::string_view str)
    {
        std::string newString = trim(str);
        if (!newString.empty()) {
            if ((newString.front() == '[') || (newString.front() == '(') ||
                (newString.front() == '{') || (newString.front() == '<')) {
                if (static_cast<unsigned char>(newString.back()) ==
                    pmap[newString.front()]) {
                    newString.pop_back();
                    newString.erase(0, 1);
                }
            }
        }
        return newString;
    }

    std::string getTailString(std::string_view input, char sep) noexcept
    {
        auto sepLoc = input.find_last_of(sep);
        auto ret = std::string(
            (sepLoc == std::string::npos) ? input : input.substr(sepLoc + 1));
        return ret;
    }

    std::string
        getTailString(std::string_view input, std::string_view sep) noexcept
    {
        auto sepLoc = input.rfind(sep);
        std::string_view ret = (sepLoc == std::string_view::npos) ?
            input :
            input.substr(sepLoc + sep.size());
        return std::string(ret);
    }

    std::string
        getTailString_any(std::string_view input, std::string_view sep) noexcept
    {
        auto sepLoc = input.find_last_of(sep);
        auto ret = std::string(
            (sepLoc == std::string::npos) ? input : input.substr(sepLoc + 1));
        return ret;
    }

    static inline bool isIsolatingChar(char testChar)
    {
        return ((isspace(testChar) != 0) || (ispunct(testChar) != 0));
    }

    static bool hasIsolatedChar(const std::string& testString, char testChar)
    {
        auto findLoc = testString.find(testChar);
        while (findLoc != std::string::npos) {
            if (findLoc == 0) {
                if (isIsolatingChar(testString[findLoc + 1])) {
                    return true;
                }
            } else if (findLoc == testString.length() - 1) {
                if (isIsolatingChar(testString[findLoc - 1])) {
                    return true;
                }
            } else if (
                isIsolatingChar(testString[findLoc - 1]) &&
                isIsolatingChar(testString[findLoc + 1])) {
                return true;
            }

            findLoc = testString.find(testChar, findLoc + 1);
        }
        return false;
    }

    static bool checkForMatch(
        const std::string& string1,
        const std::string& string2,
        string_match_type matchType)
    {
        switch (matchType) {
            case string_match_type::exact:
                if (string1 == string2) {
                    return true;
                }
                break;
            case string_match_type::begin:
                if (string2.compare(0, string1.length(), string1) == 0) {
                    return true;
                }
                break;
            case string_match_type::end:
                if (string1.length() > string2.length()) {
                    return false;
                }
                if (string2.compare(
                        string2.length() - string1.length(),
                        string1.length(),
                        string1) == 0) {
                    return true;
                }
                break;
            case string_match_type::close:
                if (string1.length() == 1)  // special case
                {  // we are checking if the single character is
                   // isolated from other alphanumeric characters
                    if (hasIsolatedChar(string2, string1.front())) {
                        return true;
                    }
                } else {
                    auto findLoc = string2.find(string1);
                    if (findLoc != std::string::npos) {
                        return true;
                    }
                    auto nstr = removeChar(string1, '_');
                    if (string2 == nstr) {
                        return true;
                    }
                    auto nstr2 = removeChar(string2, '_');
                    if ((string1 == nstr2) || (nstr == nstr2)) {
                        return true;
                    }
                }
                break;
        }
        return false;
    }

    // NOLINTNEXTLINE
    int findCloseStringMatch(
        const stringVector& testStrings,
        const stringVector& inputStrings,
        string_match_type matchType)
    {
        std::string lct;  // lower case test string
        std::string lcis;  // lower case input string
        stringVector lciStrings = inputStrings;
        // make all the input strings lower case
        for (auto& str : lciStrings) {
            makeLowerCase(str);
        }
        for (const auto& testStr : testStrings) {
            lct = convertToLowerCase(testStr);
            for (int kk = 0; kk < static_cast<int>(lciStrings.size()); ++kk) {
                lcis = lciStrings[kk];
                if (checkForMatch(lct, lcis, matchType)) {
                    return kk;
                }
            }
        }
        return -1;
    }

    std::string removeChars(std::string_view source, std::string_view remchars)
    {
        std::string result;
        result.reserve(source.length());
        std::remove_copy_if(
            source.begin(),
            source.end(),
            std::back_inserter(result),
            [remchars](char input) {
                return (
                    std::find(remchars.begin(), remchars.end(), input) !=
                    remchars.end());
            });
        return result;
    }

    std::string removeChar(std::string_view source, char remchar)
    {
        std::string result;
        result.reserve(source.length());
        std::remove_copy(
            source.begin(), source.end(), std::back_inserter(result), remchar);
        return result;
    }

    std::string characterReplace(
        std::string_view source,
        char key,
        std::string_view repStr)
    {
        std::string result;
        result.reserve(source.length());
        for (auto sourceChar : source) {
            if (sourceChar == key) {
                result += repStr;
            } else {
                result.push_back(sourceChar);
            }
        }
        return result;
    }

    std::string xmlCharacterCodeReplace(std::string str)
    {
        std::string out = std::move(str);
        auto codeLoc = out.find("&gt;");
        while (codeLoc != std::string::npos) {
            out.replace(codeLoc, 4, ">");
            codeLoc = out.find("&gt;", codeLoc + 1);
        }
        codeLoc = out.find("&lt;");
        while (codeLoc != std::string::npos) {
            out.replace(codeLoc, 4, "<");
            codeLoc = out.find("&lt;", codeLoc + 1);
        }
        codeLoc = out.find("&quot;");
        while (codeLoc != std::string::npos) {
            out.replace(codeLoc, 6, "\"");
            codeLoc = out.find("&quot;", codeLoc + 1);
        }
        codeLoc = out.find("&apos;");
        while (codeLoc != std::string::npos) {
            out.replace(codeLoc, 6, "'");
            codeLoc = out.find("&apos;", codeLoc + 1);
        }
        // &amp; is last so it can't trigger other sequences
        codeLoc = out.find("&amp;");
        while (codeLoc != std::string::npos) {
            out.replace(codeLoc, 5, "&");
            codeLoc = out.find("&amp;", codeLoc + 1);
        }
        return out;
    }
}  // namespace stringOps

std::string randomString(std::string::size_type length)
{
    static constexpr auto chars =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    thread_local static std::mt19937 rng{
        std::random_device{}() +
        static_cast<uint32_t>(
            reinterpret_cast<uint64_t>(&length) &  // NOLINT
            0xFFFFFFFFU)};
    thread_local static std::uniform_int_distribution<std::string::size_type>
        pick(0, 61);
    std::string randomString;

    randomString.reserve(length);

    while (length-- != 0U) {
        randomString.push_back(chars[pick(rng)]);
    }

    return randomString;
}

}  // namespace gmlc::utilities
