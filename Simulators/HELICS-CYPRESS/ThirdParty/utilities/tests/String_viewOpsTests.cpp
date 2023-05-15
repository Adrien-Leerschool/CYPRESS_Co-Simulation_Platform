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

#include "gmlc/utilities/string_viewOps.h"

#include "gtest/gtest.h"
#include <iostream>

using namespace gmlc::utilities::string_viewOps;

/** test trim*/
TEST(stringViewOps, trimString_tests)
{
    std::string_view test1 = "AbCd: *Ty; ";
    test1 = trim(test1);
    EXPECT_EQ(test1, "AbCd: *Ty;");
    std::string_view test2 = "  \t1234:AbC\n\t RTrt\n ";
    test2 = trim(test2);
    EXPECT_EQ(test2, "1234:AbC\n\t RTrt");
    // test for an empty results
    std::string_view test3 = "  \t\n \t\t \n ";
    test3 = trim(test3);
    EXPECT_TRUE(test3.empty());

    // test for other characters
    std::string_view test4 = "%%**Bill45 *%*%";
    test4 = trim(test4, "*%");
    EXPECT_EQ(test4, "Bill45 ");

    std::string_view test5 = "AbCd: *Ty; ";
    trimString(test5);
    EXPECT_EQ(test5, "AbCd: *Ty;");
    std::string_view test6 = "  \t1234:AbC\n\t RTrt\n ";
    trimString(test6);
    EXPECT_EQ(test6, "1234:AbC\n\t RTrt");
    // test for an empty results
    std::string_view test7 = "  \t\n \t\t \n ";
    trimString(test7);
    EXPECT_TRUE(test7.empty());

    // test for other characters
    std::string_view test8 = "%%**Bill45 *%*%";
    trimString(test8, "*%");
    EXPECT_EQ(test8, "Bill45 ");
}

TEST(stringViewOps, tailString_tests)
{
    std::string_view test1 = "AbCd: *Ty; ";
    auto testres = getTailString(test1, '*');
    EXPECT_EQ(testres, "Ty; ");
    std::string_view test2 = "bob::test1:test2:4";
    testres = getTailString(test2, ':');
    EXPECT_EQ(testres, "4");

    std::string_view test3 = "::  \t1234:AbC\n\t RTrt\n ::";
    // test with ':' also as a space
    testres = getTailString(test3, ':');
    EXPECT_TRUE(testres.empty());

    std::string_view test4 = "bob::test1:test2:4";
    testres = getTailString(test4, '-');
    EXPECT_EQ(testres, test4);

    std::string_view test5 = "4testingBeginning";
    testres = getTailString(test5, '4');
    EXPECT_EQ(testres, "testingBeginning");

    std::string_view test6 = "4testingBeginning";
    testres = getTailString(test6, 'g');
    EXPECT_TRUE(testres.empty());
}

/** test trim*/
TEST(stringViewOps, tailString_tests_word)
{
    std::string_view test1 = "AbCd: *Ty; ";
    auto testres = getTailString(test1, "*");
    EXPECT_EQ(testres, "Ty; ");
    std::string_view test2 = "bob::test1:test2:4";
    testres = getTailString(test2, ":");
    EXPECT_EQ(testres, "4");

    std::string_view test3 = "::  \t1234:AbC\n\t RTrt\n ::";
    // test with ':' also as a space
    testres = getTailString(test3, "::");
    EXPECT_TRUE(testres.empty());

    std::string_view test4 = "bob::test1:test2:4";
    testres = getTailString(test4, "-");
    EXPECT_EQ(testres, test4);

    std::string_view test5 = "4testingBeginning";
    testres = getTailString(test5, "4");
    EXPECT_EQ(testres, "testingBeginning");

    std::string_view test6 = "4testingBeginning";
    testres = getTailString(test6, "ng");
    EXPECT_TRUE(testres.empty());

    std::string_view test7 = "aba::aba::sharpaba";
    testres = getTailString(test7, "sharp");
    EXPECT_EQ(testres, "aba");

    testres = getTailString(test7, "ab");
    EXPECT_EQ(testres, "a");

    testres = getTailString(test7, std::string_view{});
    EXPECT_TRUE(testres.empty());
}

TEST(stringViewOps, tailString_any)
{
    std::string_view test1 = "AbCd: *Ty; ";
    auto testres = getTailString_any(test1, ":*");
    EXPECT_EQ(testres, "Ty; ");
    std::string_view test2 = "bob::test1:test2:4";
    testres = getTailString_any(test2, ":");
    EXPECT_EQ(testres, "4");

    std::string_view test3 = "::  \t1234:AbC\n\t RTrt\n ::";
    // test with ':' also as a space
    testres = getTailString_any(test3, " ::");
    EXPECT_TRUE(testres.empty());

    std::string_view test4 = "bob::test1:test2:4";
    testres = getTailString_any(test4, "-");
    EXPECT_EQ(testres, test4);

    std::string_view test5 = "4testingBeginning";
    testres = getTailString_any(test5, "4");
    EXPECT_EQ(testres, "testingBeginning");

    std::string_view test6 = "4testingBeginning";
    testres = getTailString_any(test6, "ing");
    EXPECT_TRUE(testres.empty());

    testres = getTailString_any(test6, std::string_view{});
    EXPECT_EQ(testres, test6);
}

/** simple split line test*/
TEST(stringViewOps, splitline_test1)
{
    std::string_view test1 = "alpha, bravo, charlie";
    auto testres = split(test1);

    ASSERT_EQ(testres.size(), 3);
    EXPECT_EQ(testres[0], "alpha");
    EXPECT_EQ(testres[1], " bravo");
    EXPECT_EQ(testres[2], " charlie");

    testres = split(test1, ", ", delimiter_compression::on);

    ASSERT_EQ(testres.size(), 3);
    EXPECT_EQ(testres[0], "alpha");
    EXPECT_EQ(testres[1], "bravo");
    EXPECT_EQ(testres[2], "charlie");
}

/** simple split line test for multiple tokens*/
TEST(stringViewOps, splitline_test2)
{
    std::string_view test1 = "alpha, bravo,;, charlie,";
    auto testres = split(test1);

    ASSERT_EQ(testres.size(), 6);
    EXPECT_TRUE(testres[2].empty());
    EXPECT_TRUE(testres[3].empty());
    EXPECT_TRUE(testres[5].empty());

    testres = split(test1, ";, ", delimiter_compression::on);

    ASSERT_EQ(testres.size(), 3);
}

/** simple split line and trim test*/
TEST(stringViewOps, splitline_test3)
{
    std::string_view test1 = " alpha,   bravo ; charlie ";
    auto testres = split(test1);
    trim(testres);
    ASSERT_EQ(testres.size(), 3);
    EXPECT_EQ(testres[0], "alpha");
    EXPECT_EQ(testres[1], "bravo");
    EXPECT_EQ(testres[2], "charlie");
}

/**remove quotes test*/
TEST(stringViewOps, removeQuotes_test)
{
    std::string_view test1 = "\'remove quotes\'";
    auto testres = removeQuotes(test1);

    EXPECT_EQ(testres, "remove quotes");
    std::string_view test2 = "\"remove quotes \"";
    testres = removeQuotes(test2);

    EXPECT_EQ(testres, "remove quotes ");

    std::string_view test3 = "\"remove quotes \'";
    testres = removeQuotes(test3);

    EXPECT_EQ(testres, "\"remove quotes \'");

    std::string_view test4 = "   \" remove quotas \"  ";
    testres = removeQuotes(test4);

    EXPECT_EQ(testres, " remove quotas ");

    std::string_view test5 = "   ` remove quotes `  ";
    testres = removeQuotes(test5);

    EXPECT_EQ(testres, " remove quotes ");
}

/**remove quotes test test*/
TEST(stringViewOps, removeBrackets_test)
{
    std::string_view test1 = "remove bracket";
    auto testres = removeBrackets(test1);

    EXPECT_EQ(testres, "remove bracket");

    std::string_view test2 = "[remove bracket]";
    testres = removeBrackets(test2);

    EXPECT_EQ(testres, "remove bracket");

    std::string_view test3 = "{remove bracket}";
    testres = removeBrackets(test3);

    EXPECT_EQ(testres, "remove bracket");

    std::string_view test4 = "<remove bracket>";
    testres = removeBrackets(test4);

    EXPECT_EQ(testres, "remove bracket");

    std::string_view test5 = "< remove bracket >";
    testres = removeBrackets(test5);

    EXPECT_EQ(testres, " remove bracket ");

    std::string_view test6 = "< remove bracket >";
    testres = removeBrackets(test6);

    EXPECT_EQ(testres, " remove bracket ");

    std::string_view test7 = " <( remove bracket )>  ";
    testres = removeBrackets(test7);

    EXPECT_EQ(testres, "( remove bracket )");

    std::string_view test8 = " <( remove bracket )}  ";
    testres = removeBrackets(test8);

    EXPECT_EQ(testres, "<( remove bracket )}");

    testres = removeBrackets(std::string_view{});

    EXPECT_TRUE(testres.empty());

    std::string tstring(100, ' ');
    testres = removeBrackets(tstring);

    EXPECT_TRUE(testres.empty());
}

TEST(stringViewOps, splitLineQuotes_tests)
{
    std::string_view test1 = "454, 345, happy; frog";
    auto testres = splitlineQuotes(test1);
    trim(testres);
    ASSERT_EQ(testres.size(), 4U);
    EXPECT_EQ(testres[0], "454");
    EXPECT_EQ(testres[1], "345");
    EXPECT_EQ(testres[2], "happy");
    EXPECT_EQ(testres[3], "frog");
    std::string_view test2 = " \'alpha,   bravo\' ; charlie ";
    auto testres2 = splitlineQuotes(test2);
    trim(testres2);
    ASSERT_EQ(testres2.size(), 2U);
    EXPECT_EQ(testres2[0], "\'alpha,   bravo\'");
    EXPECT_EQ(testres2[1], "charlie");

    std::string_view test3 = R"( "test1",'test2' ; "charlie",)";
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

    std::string_view test4 = R"("'part1' and,; 'part2'","34,45,56")";
    auto testres4 = splitlineQuotes(test4);
    ASSERT_EQ(testres4.size(), 2U);
    EXPECT_EQ(testres4[1], "\"34,45,56\"");

    std::string_view test5 = R"("part1'" and "part2","34,45,56")";
    auto testres5 = splitlineQuotes(test5);
    ASSERT_EQ(testres5.size(), 2U);
    EXPECT_EQ(testres5[1], "\"34,45,56\"");
}

TEST(stringViewOps, splitLineBracket_tests)
{
    std::string_view test1 = "(454, 345), happy; frog";
    auto testres = splitlineBracket(test1);
    trim(testres);
    ASSERT_EQ(testres.size(), 3U);
    EXPECT_EQ(testres[0], "(454, 345)");
    EXPECT_EQ(testres[1], "happy");
    EXPECT_EQ(testres[2], "frog");
    std::string_view test2 = " \'alpha,   bravo\' ; charlie ";
    // the default bracket split should recognize strings as well
    auto testres2 = splitlineBracket(test2);
    trim(testres2);
    ASSERT_EQ(testres2.size(), 2U);
    EXPECT_EQ(testres2[0], "\'alpha,   bravo\'");
    EXPECT_EQ(testres2[1], "charlie");

    std::string_view test3 = "$45,34,45$;$23.45,34,23.3$";
    auto testres3 = splitlineBracket(test3, ";,", "$");
    trim(testres3);
    ASSERT_EQ(testres3.size(), 2U);
    EXPECT_EQ(testres3[0], "$45,34,45$");
    EXPECT_EQ(testres3[1], "$23.45,34,23.3$");
}

TEST(stringViewOps, trailingInt)
{
    std::string_view name;

    std::string_view input = "bob47";
    auto val = trailingStringInt(input, name);
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

TEST(stringViewOps, trailingIntNoRet)
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

TEST(stringViewOps, mergeTest)
{
    std::string alongString = "test1";
    alongString.append("sep");
    alongString.append("test2");

    std::string_view whole(alongString);
    std::string_view part1 = whole.substr(0, 5);

    std::string_view part2 = whole.substr(8);

    std::string_view res = merge(part1, part2);
    EXPECT_EQ(res, whole);

    std::string_view empty;

    auto p3 = merge(part1, empty);
    EXPECT_EQ(p3, part1);

    p3 = merge(empty, part1);
    EXPECT_EQ(p3, part1);

    auto p4 = merge(empty, empty);
    EXPECT_TRUE(p4.empty());

    std::string lstr(100, 'a');
    std::string_view aba(lstr.c_str(), 3);
    std::string_view bab(lstr.c_str() + 80, 3);
    EXPECT_THROW(merge(aba, bab), std::out_of_range);
}
