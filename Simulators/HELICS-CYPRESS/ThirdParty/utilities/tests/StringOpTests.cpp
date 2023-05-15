/*
 * LLNS Copyright Start
 * Copyright (c) 2014-2019, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 */

#include "gmlc/utilities/stringOps.h"

#include "gtest/gtest.h"
#include <algorithm>
#include <iostream>

using namespace gmlc::utilities;
using namespace gmlc::utilities::stringOps;

/** test conversion to lower case*/
TEST(stringops, convert_to_lower_case)
{
    std::string test1 = "AbCd: *Ty; ";
    auto testres = convertToLowerCase(test1);
    EXPECT_EQ(testres, "abcd: *ty; ");
    testres = convertToLowerCase("  1234:ABC\n\t RTrt ");
    EXPECT_EQ(testres, "  1234:abc\n\t rtrt ");
}

/** test make lower case*/
TEST(stringops, make_lower_case)
{
    std::string test1 = "AbCd: *Ty; ";
    makeLowerCase(test1);
    EXPECT_EQ(test1, "abcd: *ty; ");
    std::string test2 = "  1234:ABC\n\t RTrt ";
    makeLowerCase(test2);
    EXPECT_EQ(test2, "  1234:abc\n\t rtrt ");
}

/** test conversion to upper case*/
TEST(stringops, convert_to_upper_case)
{
    std::string test1 = "AbCd: *Ty; ";
    auto testres = convertToUpperCase(test1);
    EXPECT_EQ(testres, "ABCD: *TY; ");
    testres = convertToUpperCase("  1234:ABC\n\t RTrt ");
    EXPECT_EQ(testres, "  1234:ABC\n\t RTRT ");
}

/** test make upper case*/
TEST(stringops, make_upper_case)
{
    std::string test1 = "AbCd: *Ty; ";
    makeUpperCase(test1);
    EXPECT_EQ(test1, "ABCD: *TY; ");
    std::string test2 = "  1234:ABC\n\t RTrt ";
    makeUpperCase(test2);
    EXPECT_EQ(test2, "  1234:ABC\n\t RTRT ");
}

/** test trim*/
TEST(stringops, trimString)
{
    std::string test1 = "AbCd: *Ty; ";
    trimString(test1);
    EXPECT_EQ(test1, "AbCd: *Ty;");
    std::string test2 = "  \t1234:AbC\n\t RTrt\n ";
    trimString(test2);
    EXPECT_EQ(test2, "1234:AbC\n\t RTrt");
    // test for an empty results
    std::string test3 = "  \t\n \t\t \n ";
    trimString(test3);
    EXPECT_TRUE(test3.empty());
}

/** test trim*/
TEST(stringops, trim)
{
    std::string test1 = "AbCd: *Ty; ";
    auto testres = trim(test1);
    EXPECT_EQ(testres, "AbCd: *Ty;");
    std::string test2 = "  \t1234:AbC\n\t RTrt\n ";
    testres = trim(test2);
    EXPECT_EQ(testres, "1234:AbC\n\t RTrt");

    std::string test3 = "::  \t1234:AbC\n\t RTrt\n ::";
    // test with ':' also as a space
    testres = trim(test3, "\t\n: ");
    EXPECT_EQ(testres, "1234:AbC\n\t RTrt");

    std::string test4 = "  \t\n \t\t \n ";
    testres = trim(test4);
    EXPECT_TRUE(testres.empty());
}

TEST(stringops, tailString)
{
    std::string test1 = "AbCd: *Ty; ";
    auto testres = getTailString(test1, '*');
    EXPECT_EQ(testres, "Ty; ");
    std::string test2 = "bob::test1:test2:4";
    testres = getTailString(test2, ':');
    EXPECT_EQ(testres, "4");

    std::string test3 = "::  \t1234:AbC\n\t RTrt\n ::";
    // test with ':' also as a space
    testres = getTailString(test3, ':');
    EXPECT_TRUE(testres.empty());

    std::string test4 = "bob::test1:test2:4";
    testres = getTailString(test4, '-');
    EXPECT_EQ(testres, test4);

    std::string test5 = "4testingBeginning";
    testres = getTailString(test5, '4');
    EXPECT_EQ(testres, "testingBeginning");

    std::string test6 = "4testingBeginning";
    testres = getTailString(test6, 'g');
    EXPECT_TRUE(testres.empty());
}

/** test trim*/
TEST(stringops, tailString_word)
{
    std::string test1 = "AbCd: *Ty; ";
    auto testres = getTailString(test1, "*");
    EXPECT_EQ(testres, "Ty; ");
    std::string test2 = "bob::test1:test2:4";
    testres = getTailString(test2, ":");
    EXPECT_EQ(testres, "4");

    std::string test3 = "::  \t1234:AbC\n\t RTrt\n ::";
    // test with ':' also as a space
    testres = getTailString(test3, "::");
    EXPECT_TRUE(testres.empty());

    std::string test4 = "bob::test1:test2:4";
    testres = getTailString(test4, "-");
    EXPECT_EQ(testres, test4);

    std::string test5 = "4testingBeginning";
    testres = getTailString(test5, "4");
    EXPECT_EQ(testres, "testingBeginning");

    std::string test6 = "4testingBeginning";
    testres = getTailString(test6, "ng");
    EXPECT_TRUE(testres.empty());

    std::string test7 = "aba::aba::sharpaba";
    testres = getTailString(test7, "sharp");
    EXPECT_EQ(testres, "aba");

    testres = getTailString(test7, "ab");
    EXPECT_EQ(testres, "a");

    testres = getTailString(test7, std::string{});
    EXPECT_TRUE(testres.empty());
}

TEST(stringops, tailString_any)
{
    std::string test1 = "AbCd: *Ty; ";
    auto testres = getTailString_any(test1, ":*");
    EXPECT_EQ(testres, "Ty; ");
    std::string test2 = "bob::test1:test2:4";
    testres = getTailString_any(test2, ":");
    EXPECT_EQ(testres, "4");

    std::string test3 = "::  \t1234:AbC\n\t RTrt\n ::";
    // test with ':' also as a space
    testres = getTailString_any(test3, " ::");
    EXPECT_TRUE(testres.empty());

    std::string test4 = "bob::test1:test2:4";
    testres = getTailString_any(test4, "-");
    EXPECT_EQ(testres, test4);

    std::string test5 = "4testingBeginning";
    testres = getTailString_any(test5, "4");
    EXPECT_EQ(testres, "testingBeginning");

    std::string test6 = "4testingBeginning";
    testres = getTailString_any(test6, "ing");
    EXPECT_TRUE(testres.empty());

    testres = getTailString_any(test6, std::string{});
    EXPECT_EQ(testres, test6);
}

/** simple split line test*/
TEST(stringops, splitline1)
{
    std::string test1 = "alpha, bravo, charlie";
    auto testres = splitline(test1);

    ASSERT_EQ(testres.size(), 3U);
    EXPECT_EQ(testres[0], "alpha");
    EXPECT_EQ(testres[1], " bravo");
    EXPECT_EQ(testres[2], " charlie");

    testres = splitline(test1, ", ", delimiter_compression::on);

    ASSERT_EQ(testres.size(), 3U);
    EXPECT_EQ(testres[0], "alpha");
    EXPECT_EQ(testres[1], "bravo");
    EXPECT_EQ(testres[2], "charlie");
}

/** simple split line test for multiple tokens*/
TEST(stringops, splitline2)
{
    std::string test1 = "alpha, bravo,;, charlie,";
    auto testres = splitline(test1);

    ASSERT_EQ(testres.size(), 6U);
    EXPECT_TRUE(testres[2].empty());
    EXPECT_TRUE(testres[3].empty());
    EXPECT_TRUE(testres[5].empty());

    std::string test2 = "alpha, bravo,;, charlie,";
    testres = splitline(test1, ";, ", delimiter_compression::on);

    EXPECT_EQ(testres.size(), 3U);

    // test the vector fill overload
    std::vector<std::string> resVector;
    splitline(test1, resVector, ";, ", delimiter_compression::on);
    EXPECT_EQ(resVector, testres);
}

/** simple split line test*/
TEST(stringops, splitline3)
{
    std::string test1 = " alpha,   bravo ; charlie ";
    auto testres = splitline(test1);
    trim(testres);
    ASSERT_EQ(testres.size(), 3);
    EXPECT_EQ(testres[0], "alpha");
    EXPECT_EQ(testres[1], "bravo");
    EXPECT_EQ(testres[2], "charlie");
}

/** simple split line test*/
TEST(stringops, splitline_char)
{
    std::string test1 = " alpha-bravo-charlie";
    auto testres = splitline(test1, '-');
    trim(testres);
    ASSERT_EQ(testres.size(), 3);
    EXPECT_EQ(testres[0], "alpha");
    EXPECT_EQ(testres[1], "bravo");
    EXPECT_EQ(testres[2], "charlie");

    // test the vector overload
    std::vector<std::string> resVector;
    splitline(test1, resVector, '-');
    trim(resVector);
    EXPECT_EQ(testres, resVector);
}

/**remove quotes test test*/
TEST(stringops, removeQuotes)
{
    std::string test1 = "\'remove quotes\'";
    auto testres = removeQuotes(test1);

    EXPECT_EQ(testres, "remove quotes");
    std::string test2 = "\"remove quotes \"";
    testres = removeQuotes(test2);

    EXPECT_EQ(testres, "remove quotes ");

    std::string test3 = "\"remove quotes \'";
    testres = removeQuotes(test3);

    EXPECT_EQ(testres, "\"remove quotes \'");

    std::string test4 = "   \" remove quotes \"  ";
    testres = removeQuotes(test4);

    EXPECT_EQ(testres, " remove quotes ");

    std::string test5 = "   ` remove quotes `  ";
    testres = removeQuotes(test5);

    EXPECT_EQ(testres, " remove quotes ");
}

/**remove quotes test test*/
TEST(stringops, removeBrackets)
{
    std::string test1 = "remove bracket";
    auto testres = removeBrackets(test1);

    EXPECT_EQ(testres, "remove bracket");

    std::string test2 = "[remove bracket]";
    testres = removeBrackets(test2);

    EXPECT_EQ(testres, "remove bracket");

    std::string test3 = "{remove bracket}";
    testres = removeBrackets(test3);

    EXPECT_EQ(testres, "remove bracket");

    std::string test4 = "<remove bracket>";
    testres = removeBrackets(test4);

    EXPECT_EQ(testres, "remove bracket");

    std::string test5 = "< remove bracket >";
    testres = removeBrackets(test5);

    EXPECT_EQ(testres, " remove bracket ");

    std::string test6 = "< remove bracket >";
    testres = removeBrackets(test6);

    EXPECT_EQ(testres, " remove bracket ");

    std::string test7 = " <( remove bracket )>  ";
    testres = removeBrackets(test7);

    EXPECT_EQ(testres, "( remove bracket )");

    std::string test8 = " <( remove bracket )}  ";
    testres = removeBrackets(test8);

    EXPECT_EQ(testres, "<( remove bracket )}");

    testres = removeBrackets(std::string{});

    EXPECT_TRUE(testres.empty());

    testres = removeBrackets(std::string(100, ' '));

    EXPECT_TRUE(testres.empty());
}

TEST(stringops, appendInteger)
{
    std::string str1 = "tail_";
    appendInteger(str1, 456U);
    EXPECT_EQ(str1, "tail_456");

    std::string str2 = "tail_";
    appendInteger(str2, 0);
    EXPECT_EQ(str2, "tail_0");

    std::string str3 = "tail";
    appendInteger(str3, -34);
    EXPECT_EQ(str3, "tail-34");

    std::string str4 = "tail";
    appendInteger(str4, 12345678ULL);
    EXPECT_EQ(str4, "tail12345678");

    // only use the integer part of the number if it is less than 1,000,000,000
    std::string str5 = "num=";
    appendInteger(str5, 98.2341);
    EXPECT_EQ(str5, "num=98");

    // the result here is not well defined so this check is mainly to make sure
    // it didn't crash
    std::string str6 = "num=";
    appendInteger(str6, 45e34);
    EXPECT_NE(str6, "num=");

    std::string str7 = "long num_";
    appendInteger(str7, 1234567890123456LL);
    EXPECT_EQ(str7, "long num_1234567890123456");

    std::string str8 = "long num_";
    appendInteger(str8, -1234567890123LL);
    EXPECT_EQ(str8, "long num_-1234567890123");
}

TEST(stringops, splitLineQuotes)
{
    std::string test1 = "454, 345, happy; frog";
    auto testres = splitlineQuotes(test1);
    trim(testres);
    ASSERT_EQ(testres.size(), 4U);
    EXPECT_EQ(testres[0], "454");
    EXPECT_EQ(testres[1], "345");
    EXPECT_EQ(testres[2], "happy");
    EXPECT_EQ(testres[3], "frog");
    std::string test2 = " \'alpha,   bravo\' ; charlie ";
    auto testres2 = splitlineQuotes(test2);
    trim(testres2);
    ASSERT_EQ(testres2.size(), 2U);
    EXPECT_EQ(testres2[0], "\'alpha,   bravo\'");
    EXPECT_EQ(testres2[1], "charlie");

    std::string test3 = R"( "test1",'test2' ; "charlie",)";
    auto testres3 = splitlineQuotes(test3);
    trim(testres3);
    ASSERT_EQ(testres3.size(), 4U);
    EXPECT_EQ(testres3[0], "\"test1\"");
    EXPECT_EQ(testres3[1], "\'test2\'");
    EXPECT_EQ(testres3[2], "\"charlie\"");
    EXPECT_TRUE(testres3[3].empty());

    testres3 = splitlineQuotes(
        test3,
        default_delim_chars,
        default_quote_chars,
        delimiter_compression::on);
    ASSERT_EQ(testres3.size(), 3U);

    std::string test4 = R"("'part1' and,; 'part2'","34,45,56")";
    auto testres4 = splitlineQuotes(test4);
    ASSERT_EQ(testres4.size(), 2U);
    EXPECT_EQ(testres4[1], "\"34,45,56\"");

    std::string test5 = R"("part1'" and "part2","34,45,56")";
    auto testres5 = splitlineQuotes(test5);
    ASSERT_EQ(testres5.size(), 2U);
    EXPECT_EQ(testres5[1], "\"34,45,56\"");

    std::string test6 =
        R"(--arg1 --arg2=bob --arg3="string1 string2" --arg3="bob")";
    auto testres6 = splitlineQuotes(test6, " \t");
    ASSERT_EQ(testres6.size(), 4U);
    EXPECT_EQ(testres6[2], "--arg3=\"string1 string2\"");

    std::string test7 =
        "--arg1 --arg2=bob --arg3=`string1 string2` --arg3=\"bob\"";
    auto testres7 = splitlineQuotes(test7, " \t");
    ASSERT_EQ(testres7.size(), 4U);
    EXPECT_EQ(testres7[2], "--arg3=`string1 string2`");
}

TEST(stringops, splitLineBracket)
{
    std::string test1 = "(454, 345), happy; frog";
    auto testres = splitlineBracket(test1);
    trim(testres);
    ASSERT_EQ(testres.size(), 3U);
    EXPECT_EQ(testres[0], "(454, 345)");
    EXPECT_EQ(testres[1], "happy");
    EXPECT_EQ(testres[2], "frog");
    std::string test2 = " \'alpha,   bravo\' ; charlie ";
    // the default bracket split should recognize strings as well
    auto testres2 = splitlineBracket(test2);
    trim(testres2);
    ASSERT_EQ(testres2.size(), 2);
    EXPECT_EQ(testres2[0], "\'alpha,   bravo\'");
    EXPECT_EQ(testres2[1], "charlie");

    std::string test3 = "$45,34,45$;$23.45,34,23.3$";
    auto testres3 = splitlineBracket(test3, ";,", "$");
    trim(testres3);
    ASSERT_EQ(testres3.size(), 2U);
    EXPECT_EQ(testres3[0], "$45,34,45$");
    EXPECT_EQ(testres3[1], "$23.45,34,23.3$");

    std::string test4 = ")454, 345), happy; frog";
    auto testres4 = splitlineBracket(test4);
    trim(testres4);
    EXPECT_EQ(testres4.size(), 4U);

    std::string test5 = "454, 345[, happy; frog";
    auto testres5 = splitlineBracket(test5);
    EXPECT_EQ(testres5.size(), 2U);

    std::string test6 = "[454, 3]45[, happy; frog";
    auto testres6 = splitlineBracket(test6);
    EXPECT_EQ(testres6.size(), 3U);

    std::string test7 = "454";
    auto testres7 = splitlineBracket(test7);
    EXPECT_EQ(testres7.size(), 1U);

    std::string test7b = "[454]";
    auto testres7b = splitlineBracket(test7b);
    EXPECT_EQ(testres7b.size(), 1U);

    std::string test8 = "[454, 3[45], happy]; frog";
    auto testres8 = splitlineBracket(test8);
    EXPECT_EQ(testres8.size(), 2U);
    std::string test9 = "[454, 3[45], happy];, frog";
    auto testres9 = splitlineBracket(test9);
    EXPECT_EQ(testres9.size(), 3U);

    auto testres10 = splitlineBracket(
        test9,
        default_delim_chars,
        default_bracket_chars,
        delimiter_compression::on);
    EXPECT_EQ(testres10.size(), 2U);
}

TEST(stringops, randomString)
{
    auto str1 = randomString(50000);
    std::sort(str1.begin(), str1.end());
    auto ept = std::unique(str1.begin(), str1.end());
    EXPECT_EQ(ept - str1.begin(), 62U);

    std::vector<std::string> rstring;
    rstring.reserve(20);
    for (int ii = 0; ii < 20; ++ii) {
        rstring.push_back(randomString(10));
    }

    std::sort(rstring.begin(), rstring.end());
    auto eptS = std::unique(rstring.begin(), rstring.end());
    EXPECT_EQ(eptS - rstring.begin(), 20);
}

TEST(stringops, trailingInt)
{
    std::string name;

    auto val = trailingStringInt("bob47", name);
    EXPECT_EQ(val, 47);
    EXPECT_EQ(name, "bob");

    val = trailingStringInt("bob", name);
    EXPECT_EQ(val, -1);
    EXPECT_EQ(name, "bob");
    val = trailingStringInt("bob", name, -45);
    EXPECT_EQ(val, -45);
    EXPECT_EQ(name, "bob");

    val = trailingStringInt(std::string{}, name);
    EXPECT_EQ(val, -1);
    EXPECT_TRUE(name.empty());

    val = trailingStringInt("457", name);
    EXPECT_EQ(val, 457);
    EXPECT_TRUE(name.empty());

    val = trailingStringInt("pos5", name);
    EXPECT_EQ(val, 5);
    EXPECT_EQ(name, "pos");

    val = trailingStringInt("pos_45", name);
    EXPECT_EQ(val, 45);
    EXPECT_EQ(name, "pos");

    val = trailingStringInt("pos#45", name);
    EXPECT_EQ(val, 45);
    EXPECT_EQ(name, "pos");

    val = trailingStringInt("1234567890123456789", name);
    EXPECT_EQ(val, 123456789);
    EXPECT_EQ(name, "1234567890");

    val = trailingStringInt("abab1234567890123456789", name);
    EXPECT_EQ(val, 123456789);
    EXPECT_EQ(name, "abab1234567890");
}

TEST(stringops, trailingIntNoRet)
{
    auto val = trailingStringInt("bob47");
    EXPECT_EQ(val, 47);

    val = trailingStringInt("bob");
    EXPECT_EQ(val, -1);
    val = trailingStringInt("bob", -45);
    EXPECT_EQ(val, -45);

    val = trailingStringInt(std::string{});
    EXPECT_EQ(val, -1);

    val = trailingStringInt("457");
    EXPECT_EQ(val, 457);

    val = trailingStringInt("pos5");
    EXPECT_EQ(val, 5);

    val = trailingStringInt("pos_45");
    EXPECT_EQ(val, 45);

    val = trailingStringInt("pos#45");
    EXPECT_EQ(val, 45);

    val = trailingStringInt("1234567890123456789");
    EXPECT_EQ(val, 123456789);
    val = trailingStringInt("abab1234567890123456789");
    EXPECT_EQ(val, 123456789);
}

TEST(stringops, removeChar)
{
    auto res = removeChar("happyDay", 'a');
    EXPECT_EQ(res, "hppyDy");

    res = removeChar("    ", ' ');
    EXPECT_TRUE(res.empty());

    res = removeChar(std::string{}, ' ');
    EXPECT_TRUE(res.empty());

    res = removeChar("_inter_e_stin_g", '_');
    EXPECT_EQ(res, "interesting");
}

TEST(stringops, removeChars)
{
    auto res = removeChars("happyDay", "a");
    EXPECT_EQ(res, "hppyDy");

    res = removeChars("happyDay", "pa");
    EXPECT_EQ(res, "hyDy");
    res = removeChars("happyDay", "pay");
    EXPECT_EQ(res, "hD");

    res = removeChars("    ", " ");
    EXPECT_TRUE(res.empty());

    res = removeChars(std::string{}, " ");
    EXPECT_TRUE(res.empty());

    res = removeChars("_inter_e_stin_g", "_");
    EXPECT_EQ(res, "interesting");
}

TEST(stringops, characterReplace)
{
    auto res = characterReplace("happyDay#", '#', "_Today");
    EXPECT_EQ(res, "happyDay_Today");

    res = characterReplace("#happyDay", '#', "TodayIsA");
    EXPECT_EQ(res, "TodayIsAhappyDay");

    res = characterReplace("_happy_Day_", '_', "");
    EXPECT_EQ(res, "happyDay");

    res = characterReplace("_happy_Day_", '_', "");
    EXPECT_EQ(res, "happyDay");
}

TEST(stringops, findClosestMatch)
{
    stringVector iString{"stringabcd", "Abcd2", "Abracabcdabra"};

    int res = findCloseStringMatch({"abcd"}, iString, string_match_type::begin);
    EXPECT_EQ(res, 1);
    res = findCloseStringMatch({"abcd"}, iString, string_match_type::exact);
    EXPECT_EQ(res, -1);
    res = findCloseStringMatch({"abcd"}, iString, string_match_type::end);
    EXPECT_EQ(res, 0);
    res = findCloseStringMatch({"cabc"}, iString, string_match_type::close);
    EXPECT_EQ(res, 2);
    res = findCloseStringMatch({"abcd"}, iString, string_match_type::close);
    EXPECT_EQ(res, 0);
}

TEST(stringops, findClosestMatch2)
{
    stringVector iString{
        "stringabcd", "Abcd2_i", "Abracabcdabra", "n idea", "having 8;idea"};

    int res = findCloseStringMatch(
        {"abcd", "ABcd2_i"}, iString, string_match_type::exact);
    EXPECT_EQ(res, 1);
    res = findCloseStringMatch(
        {"invalidity", "abcd"}, iString, string_match_type::exact);
    EXPECT_EQ(res, -1);
    res = findCloseStringMatch(
        {"invalidity", "abcd"}, iString, string_match_type::end);
    EXPECT_EQ(res, 0);
    res = findCloseStringMatch({"A"}, iString, string_match_type::close);
    EXPECT_EQ(res, -1);
    res = findCloseStringMatch({"i"}, iString, string_match_type::close);
    EXPECT_EQ(res, 1);
    res = findCloseStringMatch({"n"}, iString, string_match_type::close);
    EXPECT_EQ(res, 3);
    res = findCloseStringMatch({"8"}, iString, string_match_type::close);
    EXPECT_EQ(res, 4);
    res = findCloseStringMatch({"abcd2i"}, iString, string_match_type::close);
    EXPECT_EQ(res, 1);
    res = findCloseStringMatch(
        {"string_abcd"}, iString, string_match_type::close);
    EXPECT_EQ(res, 0);
}

TEST(stringops, xmlcharacterCodes)
{
    EXPECT_EQ(xmlCharacterCodeReplace("&lt;code&gt;"), "<code>");
    EXPECT_EQ(xmlCharacterCodeReplace("&lt;&lt;code&gt;&gt;"), "<<code>>");
    EXPECT_EQ(xmlCharacterCodeReplace("&quot;code&quot;"), "\"code\"");
    EXPECT_EQ(xmlCharacterCodeReplace("&inv;code&quot;"), "&inv;code\"");
    EXPECT_EQ(xmlCharacterCodeReplace("&apos;code&apos;"), "\'code\'");
    EXPECT_EQ(xmlCharacterCodeReplace("&amp;code&amp;"), "&code&");
    EXPECT_EQ(xmlCharacterCodeReplace("&amp;amp;"), "&amp;");
    EXPECT_EQ(xmlCharacterCodeReplace("&amp;lt;"), "&lt;");
    EXPECT_EQ(xmlCharacterCodeReplace("&amp;gt;"), "&gt;");
    EXPECT_EQ(xmlCharacterCodeReplace("&amp;quot;"), "&quot;");
    EXPECT_EQ(xmlCharacterCodeReplace("&amp;apos;"), "&apos;");
}
