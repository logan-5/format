#include "converter.hpp"
#include "format.hpp"
#include "user_defined.hpp"

#include <string>
#include <string_view>

#include <catch.hpp>

TEMPLATE_TEST_CASE("formatted_size", "", char, wchar_t) {
    using namespace std::literals;
    using lrstd::formatted_size;

    str_fn<TestType> str;

    {
        CHECK(formatted_size(str("{}"), str("hey")) == 3);
        CHECK(formatted_size(str("{0: ^6}"), str("hey")) == 6);
        CHECK(formatted_size(str("{:_>6}"), str("hey")) == 6);
    }
    {
        CHECK(formatted_size(str("{:s}"), true) == 4);
        CHECK(formatted_size(str("{:s}"), false) == 5);
        CHECK(formatted_size(str("{0: ^6s}"), true) == 6);
        CHECK(formatted_size(str("{:_>6s}"), false) == 6);
    }

    CHECK(formatted_size(str("{0}-{{"), 8) == 3);

    CHECK(formatted_size(str("{} to {}"), str("a"), str("b")) == 6);
    CHECK(formatted_size(str("{1} to {0}"), str("a"), str("b")) == 6);

    {
        char c = 120;
        CHECK(formatted_size(str("{:6}"), 42) == 6);
        CHECK(formatted_size(str("{:6}"), str('x')) == 6);
        CHECK(formatted_size(str("{:*<6}"), str('x')) == 6);
        CHECK(formatted_size(str("{:*>6}"), str('x')) == 6);
        CHECK(formatted_size(str("{:*^6}"), str('x')) == 6);
        CHECK(formatted_size(str("{:*^{}}"), str('x'), 6) == 6);
        CHECK(formatted_size(str("{:6d}"), c) == 6);
        CHECK(formatted_size(str("{:6}"), true) == 6);
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
        CHECK(formatted_size(str("{:+06d}"), c) == 6);
        CHECK(formatted_size(str("{:#06x}"), 0xa) == 6);
        CHECK(formatted_size(str("{:<06}"), -42) == 6);
    }

    {
        CHECK(formatted_size(str("{}"), 42) == 2);
        CHECK(formatted_size(str("{0:b} {0:d} {0:o} {0:x}"), 42) == 15);
        CHECK(formatted_size(str("{0:#x} {0:#X}"), 42) == 9);

        // clang-format off
        // TODO
        // CHECK(format("{:L}", 1234) == "1,234");  // (depending on the locale)
        // clang-format on
    }
    { CHECK(formatted_size(str("{}"), red) == 3); }
    { CHECK(formatted_size(str("{0:{1}}"), S{42}, 10) == 10); }
}
