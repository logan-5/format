#include "converter.hpp"
#include "format.hpp"

#include <catch.hpp>

TEMPLATE_TEST_CASE("hello_world", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;
    CHECK(format(str("{}ello, {}orld\n"), str('h'), str('w')) ==
          str("hello, world\n"));
    CHECK(format(str("{0}ello, {1}orld\n"), str('h'), str('w')) ==
          str("hello, world\n"));
    CHECK(format(str("{1}ello, {0}orld\n"), str('h'), str('w')) ==
          str("wello, horld\n"));
    CHECK(format(str("{1:}ello, {0:}orld\n"), str('h'), str('w')) ==
          str("wello, horld\n"));
    CHECK(format(str("{:}ello, {:}orld\n"), str('h'), str('w')) ==
          str("hello, world\n"));
}
