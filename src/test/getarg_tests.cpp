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
    ResetArgs("-DRS");
    BOOST_CHECK(GetBoolArg("-DRS"));
    BOOST_CHECK(GetBoolArg("-DRS", false));
    BOOST_CHECK(GetBoolArg("-DRS", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-DRSo"));
    BOOST_CHECK(!GetBoolArg("-DRSo", false));
    BOOST_CHECK(GetBoolArg("-DRSo", true));

    ResetArgs("-DRS=0");
    BOOST_CHECK(!GetBoolArg("-DRS"));
    BOOST_CHECK(!GetBoolArg("-DRS", false));
    BOOST_CHECK(!GetBoolArg("-DRS", true));

    ResetArgs("-DRS=1");
    BOOST_CHECK(GetBoolArg("-DRS"));
    BOOST_CHECK(GetBoolArg("-DRS", false));
    BOOST_CHECK(GetBoolArg("-DRS", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noDRS");
    BOOST_CHECK(!GetBoolArg("-DRS"));
    BOOST_CHECK(!GetBoolArg("-DRS", false));
    BOOST_CHECK(!GetBoolArg("-DRS", true));

    ResetArgs("-noDRS=1");
    BOOST_CHECK(!GetBoolArg("-DRS"));
    BOOST_CHECK(!GetBoolArg("-DRS", false));
    BOOST_CHECK(!GetBoolArg("-DRS", true));

    ResetArgs("-DRS -noDRS");  // -DRS should win
    BOOST_CHECK(GetBoolArg("-DRS"));
    BOOST_CHECK(GetBoolArg("-DRS", false));
    BOOST_CHECK(GetBoolArg("-DRS", true));

    ResetArgs("-DRS=1 -noDRS=1");  // -DRS should win
    BOOST_CHECK(GetBoolArg("-DRS"));
    BOOST_CHECK(GetBoolArg("-DRS", false));
    BOOST_CHECK(GetBoolArg("-DRS", true));

    ResetArgs("-DRS=0 -noDRS=0");  // -DRS should win
    BOOST_CHECK(!GetBoolArg("-DRS"));
    BOOST_CHECK(!GetBoolArg("-DRS", false));
    BOOST_CHECK(!GetBoolArg("-DRS", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--DRS=1");
    BOOST_CHECK(GetBoolArg("-DRS"));
    BOOST_CHECK(GetBoolArg("-DRS", false));
    BOOST_CHECK(GetBoolArg("-DRS", true));

    ResetArgs("--noDRS=1");
    BOOST_CHECK(!GetBoolArg("-DRS"));
    BOOST_CHECK(!GetBoolArg("-DRS", false));
    BOOST_CHECK(!GetBoolArg("-DRS", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DRS", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DRS", "eleven"), "eleven");

    ResetArgs("-DRS -bar");
    BOOST_CHECK_EQUAL(GetArg("-DRS", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DRS", "eleven"), "");

    ResetArgs("-DRS=");
    BOOST_CHECK_EQUAL(GetArg("-DRS", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DRS", "eleven"), "");

    ResetArgs("-DRS=11");
    BOOST_CHECK_EQUAL(GetArg("-DRS", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-DRS", "eleven"), "11");

    ResetArgs("-DRS=eleven");
    BOOST_CHECK_EQUAL(GetArg("-DRS", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-DRS", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DRS", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-DRS", 0), 0);

    ResetArgs("-DRS -bar");
    BOOST_CHECK_EQUAL(GetArg("-DRS", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-DRS=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-DRS", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-DRS=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-DRS", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--DRS");
    BOOST_CHECK_EQUAL(GetBoolArg("-DRS"), true);

    ResetArgs("--DRS=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-DRS", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noDRS");
    BOOST_CHECK(!GetBoolArg("-DRS"));
    BOOST_CHECK(!GetBoolArg("-DRS", true));
    BOOST_CHECK(!GetBoolArg("-DRS", false));

    ResetArgs("-noDRS=1");
    BOOST_CHECK(!GetBoolArg("-DRS"));
    BOOST_CHECK(!GetBoolArg("-DRS", true));
    BOOST_CHECK(!GetBoolArg("-DRS", false));

    ResetArgs("-noDRS=0");
    BOOST_CHECK(GetBoolArg("-DRS"));
    BOOST_CHECK(GetBoolArg("-DRS", true));
    BOOST_CHECK(GetBoolArg("-DRS", false));

    ResetArgs("-DRS --noDRS");
    BOOST_CHECK(GetBoolArg("-DRS"));

    ResetArgs("-noDRS -DRS"); // DRS always wins:
    BOOST_CHECK(GetBoolArg("-DRS"));
}

BOOST_AUTO_TEST_SUITE_END()
