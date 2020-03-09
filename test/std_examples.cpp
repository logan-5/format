#include "format.hpp"

#include "converter.hpp"
#include "locales.hpp"
#include "user_defined.hpp"

#include <catch.hpp>

TEMPLATE_TEST_CASE("std_examples", "", char, wchar_t) {
    using lrstd::format;

    str_fn<TestType> str;

    CHECK(format(str("{0}-{{"), 8) == str("8-{"));

    CHECK(format(str("{} to {}"), str("a"), str("b")) == str("a to b"));
    CHECK(format(str("{1} to {0}"), str("a"), str("b")) == str("b to a"));

    CHECK_THROWS_AS(format(str("{0} to {}"), str("a"), str("b")),
                    lrstd::format_error);
    CHECK_THROWS_AS(format(str("{} to {1}"), str("a"), str("b")),
                    lrstd::format_error);

    {
        TestType c = 120;
        CHECK(format(str("{:6}"), 42) == str("    42"));
        CHECK(format(str("{:6}"), str('x')) == str("x     "));
        CHECK(format(str("{:*<6}"), str('x')) == str("x*****"));
        CHECK(format(str("{:*>6}"), str('x')) == str("*****x"));
        CHECK(format(str("{:*^6}"), str('x')) == str("**x***"));
        CHECK(format(str("{:*^{}}"), str('x'), 6) == str("**x***"));
        CHECK(format(str("{:6d}"), c) == str("   120"));
        CHECK(format(str("{:6}"), true) == str("true  "));
    }

    {
          // clang-format off

        // TODO
        // double inf = std::numeric_limits<double>::infinity();
        // double nan = std::numeric_limits<double>::quiet_NaN();
        // CHECK(format("{0:},{0:+},{0:-},{0: }", 1) == "1,+1,1, 1");
        // CHECK(format("{0:},{0:+},{0:-},{0: }", -1) == "-1,-1,-1,-1");
        // CHECK(format("{0:},{0:+},{0:-},{0: }", inf) == "inf,+inf,inf, inf");
        // CHECK(format("{0:},{0:+},{0:-},{0: }", nan) == "nan,+nan,nan, nan");

          // clang-format on
    }

    {
        TestType c = 120;
        CHECK(format(str("{:+06d}"), c) == str("+00120"));
        CHECK(format(str("{:#06x}"), 0xa) == str("0x000a"));
        CHECK(format(str("{:<06}"), -42) == str("-42   "));
    }

    {
        CHECK(format(str("{}"), 42) == str("42"));
        CHECK(format(str("{0:b} {0:d} {0:o} {0:x}"), 42) ==
              str("101010 42 52 2a"));
        CHECK(format(str("{0:#x} {0:#X}"), 42) == str("0x2a 0X2A"));

        std::locale::global(en_US_locale{}());
        CHECK(format("{:L}", 1234) == "1,234");
    }
    { CHECK(format(str("{}"), red) == str("red")); }
    { CHECK(format(str("{0:{1}}"), S{42}, 10) == str("xxxxxxxx42")); }
}
