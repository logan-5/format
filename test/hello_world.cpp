#include "format.hpp"

#include <catch.hpp>

TEST_CASE("hello_world", "") {
    using lrstd::format;
    REQUIRE(format("{}ello, {}orld\n", 'h', 'w') == "hello, world\n");
    REQUIRE(format("{0}ello, {1}orld\n", 'h', 'w') == "hello, world\n");
    REQUIRE(format("{1}ello, {0}orld\n", 'h', 'w') == "wello, horld\n");
    REQUIRE(format("{1:}ello, {0:}orld\n", 'h', 'w') == "wello, horld\n");
    REQUIRE(format("{:}ello, {:}orld\n", 'h', 'w') == "hello, world\n");
}
