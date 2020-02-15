#include "format.hpp"

#include <catch.hpp>

TEST_CASE("hello_world", "") {
    using lrstd::format;
    CHECK(format("{}ello, {}orld\n", 'h', 'w') == "hello, world\n");
    CHECK(format("{0}ello, {1}orld\n", 'h', 'w') == "hello, world\n");
    CHECK(format("{1}ello, {0}orld\n", 'h', 'w') == "wello, horld\n");
    CHECK(format("{1:}ello, {0:}orld\n", 'h', 'w') == "wello, horld\n");
    CHECK(format("{:}ello, {:}orld\n", 'h', 'w') == "hello, world\n");
}
