#include "format.hpp"

#include <iostream>

#include <catch.hpp>

TEST_CASE("make_format_args", "") {
    using lrstd::format;

    REQUIRE(format("{:*^{}}", 'x', 6) == "**x***");

    lrstd::make_format_args('1', '2', '3', '4', '5');

    REQUIRE(format("{}ello, {}orld\n", 'h', 'w') == "hello, world\n");
    REQUIRE(format("{0}ello, {1}orld\n", 'h', 'w') == "hello, world\n");
    REQUIRE(format("{1}ello, {0}orld\n", 'h', 'w') == "wello, horld\n");
    REQUIRE(format("{1:}ello, {0:}orld\n", 'h', 'w') == "wello, horld\n");
    REQUIRE(format("{:}ello, {:}orld\n", 'h', 'w') == "hello, world\n");

    {
        char c = 120;
        // REQUIRE(format("{:6}", 42) == "    42");
        REQUIRE(format("{:6}", 'x') == "x     ");
        REQUIRE(format("{:*<6}", 'x') == "x*****");
        REQUIRE(format("{:*>6}", 'x') == "*****x");
        REQUIRE(format("{:*^6}", 'x') == "**x***");
        REQUIRE(format("{:*^{}}", 'x', 6) == "**x***");
        //   REQUIRE(format("{:=6}", 'x');    // Error: '=' with charT and no
        //   integer presentation type

        // TODO
        // REQUIRE(format("{:6d}", c) == "   120");
        // REQUIRE(format("{:=+06d}", c) == "+00120")
        // REQUIRE(format("{:0=#6x}", 0xa) == "0x000a");
        // REQUIRE(format("{:6}", true) == "true  ");
    }
}
