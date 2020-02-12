#include "format.hpp"

#include <iostream>

#include <catch.hpp>

TEST_CASE("make_format_args", "") {
    lrstd::make_format_args('1', '2', '3', '4', '5');

    REQUIRE(lrstd::format("{}ello, {}orld\n", 'h', 'w') == "hello, world\n");
    REQUIRE(lrstd::format("{0}ello, {1}orld\n", 'h', 'w') == "hello, world\n");
    REQUIRE(lrstd::format("{1}ello, {0}orld\n", 'h', 'w') == "wello, horld\n");
    REQUIRE(lrstd::format("{1:}ello, {0:}orld\n", 'h', 'w') ==
            "wello, horld\n");
    REQUIRE(lrstd::format("{:}ello, {:}orld\n", 'h', 'w') == "hello, world\n");
}
