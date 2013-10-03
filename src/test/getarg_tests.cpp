#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-MTK");
    BOOST_CHECK(GetBoolArg("-MTK"));
    BOOST_CHECK(GetBoolArg("-MTK", false));
    BOOST_CHECK(GetBoolArg("-MTK", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-MTKo"));
    BOOST_CHECK(!GetBoolArg("-MTKo", false));
    BOOST_CHECK(GetBoolArg("-MTKo", true));

    ResetArgs("-MTK=0");
    BOOST_CHECK(!GetBoolArg("-MTK"));
    BOOST_CHECK(!GetBoolArg("-MTK", false));
    BOOST_CHECK(!GetBoolArg("-MTK", true));

    ResetArgs("-MTK=1");
    BOOST_CHECK(GetBoolArg("-MTK"));
    BOOST_CHECK(GetBoolArg("-MTK", false));
    BOOST_CHECK(GetBoolArg("-MTK", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noMTK");
    BOOST_CHECK(!GetBoolArg("-MTK"));
    BOOST_CHECK(!GetBoolArg("-MTK", false));
    BOOST_CHECK(!GetBoolArg("-MTK", true));

    ResetArgs("-noMTK=1");
    BOOST_CHECK(!GetBoolArg("-MTK"));
    BOOST_CHECK(!GetBoolArg("-MTK", false));
    BOOST_CHECK(!GetBoolArg("-MTK", true));

    ResetArgs("-MTK -noMTK");  // -MTK should win
    BOOST_CHECK(GetBoolArg("-MTK"));
    BOOST_CHECK(GetBoolArg("-MTK", false));
    BOOST_CHECK(GetBoolArg("-MTK", true));

    ResetArgs("-MTK=1 -noMTK=1");  // -MTK should win
    BOOST_CHECK(GetBoolArg("-MTK"));
    BOOST_CHECK(GetBoolArg("-MTK", false));
    BOOST_CHECK(GetBoolArg("-MTK", true));

    ResetArgs("-MTK=0 -noMTK=0");  // -MTK should win
    BOOST_CHECK(!GetBoolArg("-MTK"));
    BOOST_CHECK(!GetBoolArg("-MTK", false));
    BOOST_CHECK(!GetBoolArg("-MTK", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--MTK=1");
    BOOST_CHECK(GetBoolArg("-MTK"));
    BOOST_CHECK(GetBoolArg("-MTK", false));
    BOOST_CHECK(GetBoolArg("-MTK", true));

    ResetArgs("--noMTK=1");
    BOOST_CHECK(!GetBoolArg("-MTK"));
    BOOST_CHECK(!GetBoolArg("-MTK", false));
    BOOST_CHECK(!GetBoolArg("-MTK", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MTK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MTK", "eleven"), "eleven");

    ResetArgs("-MTK -bar");
    BOOST_CHECK_EQUAL(GetArg("-MTK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MTK", "eleven"), "");

    ResetArgs("-MTK=");
    BOOST_CHECK_EQUAL(GetArg("-MTK", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-MTK", "eleven"), "");

    ResetArgs("-MTK=11");
    BOOST_CHECK_EQUAL(GetArg("-MTK", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-MTK", "eleven"), "11");

    ResetArgs("-MTK=eleven");
    BOOST_CHECK_EQUAL(GetArg("-MTK", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-MTK", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-MTK", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-MTK", 0), 0);

    ResetArgs("-MTK -bar");
    BOOST_CHECK_EQUAL(GetArg("-MTK", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-MTK=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-MTK", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-MTK=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-MTK", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--MTK");
    BOOST_CHECK_EQUAL(GetBoolArg("-MTK"), true);

    ResetArgs("--MTK=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-MTK", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noMTK");
    BOOST_CHECK(!GetBoolArg("-MTK"));
    BOOST_CHECK(!GetBoolArg("-MTK", true));
    BOOST_CHECK(!GetBoolArg("-MTK", false));

    ResetArgs("-noMTK=1");
    BOOST_CHECK(!GetBoolArg("-MTK"));
    BOOST_CHECK(!GetBoolArg("-MTK", true));
    BOOST_CHECK(!GetBoolArg("-MTK", false));

    ResetArgs("-noMTK=0");
    BOOST_CHECK(GetBoolArg("-MTK"));
    BOOST_CHECK(GetBoolArg("-MTK", true));
    BOOST_CHECK(GetBoolArg("-MTK", false));

    ResetArgs("-MTK --noMTK");
    BOOST_CHECK(GetBoolArg("-MTK"));

    ResetArgs("-noMTK -MTK"); // MTK always wins:
    BOOST_CHECK(GetBoolArg("-MTK"));
}

BOOST_AUTO_TEST_SUITE_END()
