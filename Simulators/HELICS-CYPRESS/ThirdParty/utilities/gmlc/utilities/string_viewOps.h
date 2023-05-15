#pragma once
/*                                                                                                   \
 * LLNS Copyright Start \
 * Copyright (c) 2014-2018, Lawrence Livermore National Security \
 * This work was performed under the auspices of the U.S. Department \
 * of Energy by Lawrence Livermore National Laboratory in part under \
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344. \
 * Produced at the Lawrence Livermore National Laboratory. \
 * All rights reserved. \
 * For details, see the LICENSE file. \
 * LLNS Copyright End \
 */

/*
Copyright (c) 2017-2022,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include <string_view>
#include <vector>

namespace gmlc::utilities {

using string_viewVector = std::vector<std::string_view>;

namespace string_viewOps {

    /** a set of white space characters*/
    constexpr std::string_view whiteSpaceCharacters(" \t\n\r\0\v\f");

    /** @brief trim specified (default=whitespace) characters from the beginning
and end of the input string
@param[in,out] input  the string to convert
@param[in] trimCharacters the characters to potentially trim
*/
    void trimString(
        std::string_view& input,
        std::string_view trimCharacters = whiteSpaceCharacters);

    /** @brief trim characters (default=whitespace) from a string
@param[in] input the string to trim
@param[in] trimCharacters  the set of characters to trim; defaults to
\t\n\r\0\v\f
@return the trimmed string
*/
    std::string_view trim(
        std::string_view input,
        std::string_view trimCharacters = whiteSpaceCharacters);

    /** @brief trim the specified characters from a vector of std::string_views
@param[in] input the vector of strings to trim;
@param[in] trimCharacters  the set of characters to trim; defaults to
\t\n\r\0\v\f
*/
    void trim(
        string_viewVector& input,
        std::string_view trimCharacters = whiteSpaceCharacters);

    /** @brief remove outer quotes from a string
only quotes around the edges are removed along with whitespace outside the
quotes; whitespace is also trimmed
@param[in] str  the original string
@return  the string with quotes removed
*/
    std::string_view removeQuotes(std::string_view str);

    /** @brief remove outer brackets from a string
outer brackets include [({<  and their matching pair; whitespace is also
trimmmed
@param[in] str  the original string
@return  the string with outer brackets removed
*/
    std::string_view removeBrackets(std::string_view str);

    /** @brief get a string that comes after the last of a specified
separator
@param[in] input  the string to separate
@param[in] separationCharacter the separation character
@return  the tail string or the string that comes after the last separation
character; if not found returns the entire string
*/
    std::string_view getTailString(
        std::string_view input,
        char separationCharacter) noexcept;

    /** @brief get a stringView that comes after the last of any of the
specified separator
@param[in] input  the string to separate
@param[in] separationCharacters the separation characters
@return  the tail string or the string that comes after the last separation
character; if not found returns the entire string
*/
    std::string_view getTailString_any(
        std::string_view input,
        std::string_view separationCharacters) noexcept;

    /** @brief get a string that comes after the last of the specified
separator
@param input  the string to separate
@param sep a character sequence indicating the tail
@return  the tail string or the string that comes after the last of the
separation strings; if not found returns the entire string
*/
    std::string_view
        getTailString(std::string_view input, std::string_view sep) noexcept;

    constexpr std::string_view default_delim_chars(",;");
    constexpr std::string_view default_quote_chars(R"raw('"`)raw");
    constexpr std::string_view default_bracket_chars(R"raw([{(<'"`)raw");

    enum class delimiter_compression {
        on,
        off,
    };

    /** @brief split a line into a vector of stringViews
@param[in] str the string to split
@param[in] delimiters a string containing the valid delimiter characters
@param[in] compression default off,  if set to delimiter_compression::on will
merge multiple sequential delimiters together
@return a vector of strings separated by the delimiters characters
*/

    string_viewVector split(
        std::string_view str,
        std::string_view delimiters = default_delim_chars,
        delimiter_compression compression = delimiter_compression::off);
    /** @brief split a line into a vector of strings taking into account
quote characters the delimiter characters are allowed inside the brackets and
the resulting vector will take the brackets into account
@param[in] line  the string to split
@param[in]  delimiters a string containing the valid delimiter characters
@param[in] compression default off,  if set to delimiter_compression::on will
merge multiple sequential delimiters together
@return a vector of strings separated by the delimiters characters accounting
for bracketing characters
*/
    string_viewVector splitlineQuotes(
        std::string_view line,
        std::string_view delimiters = default_delim_chars,
        std::string_view quoteChars = default_quote_chars,
        delimiter_compression compression = delimiter_compression::off);

    /** @brief split a line into a vector of strings taking into account
bracketing characters bracket characters include "()","{}","[]","<>" as well as
quote characters ' and " the delimiter characters are allowed inside the
brackets and the resulting vector will take the brackets into account
@param[in] line  the string to spit
@param[in]  delimiters a string containing the valid delimiter characters
@param[in] compression default off,  if set to delimiter_compression::on will
merge multiple sequential delimiters together
@return a vector of strings separated by the delimiters characters accounting
for bracketing characters
*/
    string_viewVector splitlineBracket(
        std::string_view line,
        std::string_view delimiters = default_delim_chars,
        std::string_view bracketChars = default_bracket_chars,
        delimiter_compression compression = delimiter_compression::off);

    /** @brief merge two stringViews together
@details this will only work if these are from a single original string
the check is the gap between the start of one string and another is less than 24
characters.  The intended use case is if a string view was split by comma or
other separator but needed to be combined again later
@param string1 the first string
@param string2 the second string
@return a new string view of the combined string
@throws an out_of_range exception if the string cannot be merged
*/
    std::string_view merge(std::string_view string1, std::string_view string2);

    /** @brief extract a trailing number from a string return the number and
the string without the number
@param[in] input the string to extract the information from
@param[out] output the leading string with the numbers removed
@param[in] defNum the default number to return if no trailing number was found
@return the numerical value of the trailing number*/
    int trailingStringInt(
        std::string_view input,
        std::string_view& output,
        int defNum = -1);

    /** @brief extract a trailing number from a string
@param[in] input the string to extract the information from
@param[in] defNum the default number to return if no trailing number was found
@return the numerical value of the trailing number*/
    int trailingStringInt(std::string_view input, int defNum = -1);
    /** convert a stringView to an integer
@details this function does not handle any complexities it ignores all non digit
characters including negative signs and returns a positive integer or 0
basically the number you would get if all non integer characters were erased*/
    int toIntSimple(std::string_view input);
}  // namespace string_viewOps
}  // namespace gmlc::utilities
