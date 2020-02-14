#include "format.hpp"

#include <catch.hpp>

enum color { red, green, blue };
const char* color_names[] = {"red", "green", "blue"};

template <>
struct lrstd::formatter<color> : lrstd::formatter<const char*> {
    auto format(color c, format_context& ctx) {
        return formatter<const char*>::format(color_names[c], ctx);
    }
};

struct S {
    int value;
};
template <>
struct lrstd::formatter<S> {
    size_t width_arg_id = 0;

    // Parses a width argument id in the format { digit }.
    constexpr auto parse(format_parse_context& ctx) {
        auto iter = ctx.begin();
        auto get_char = [&]() { return iter != ctx.end() ? *iter : 0; };
        if (get_char() != '{')
            return iter;
        ++iter;
        char c = get_char();
        if (!isdigit(c) || (++iter, get_char()) != '}')
            throw format_error("invalid format");
        width_arg_id = c - '0';
        ctx.check_arg_id(width_arg_id);
        return ++iter;
    }

    // Formats an S with width given by the argument width_­arg_­id.
    auto format(S s, format_context& ctx) {
        int width = visit_format_arg(
              [](auto value) -> int {
                  if constexpr (!std::is_integral_v<decltype(value)>)
                      throw format_error("width is not integral");
                  else if (value < 0 || value > std::numeric_limits<int>::max())
                      throw format_error("invalid width");
                  else
                      return value;
              },
              ctx.arg(width_arg_id));
        return format_to(ctx.out(), "{0:x>{1}}", s.value, width);
    }
};

TEST_CASE("std_examples", "") {
    using lrstd::format;

    REQUIRE(format("{0}-{{", 8) == "8-{");

    REQUIRE(format("{} to {}", "a", "b") == "a to b");
    REQUIRE(format("{1} to {0}", "a", "b") == "b to a");
    {
        bool threw = false;
        try {
            format("{0} to {}", "a", "b");
        } catch (const lrstd::format_error&) {
            threw = true;
        }
        REQUIRE(threw);
    }
    {
        bool threw = false;
        try {
            format("{} to {1}", "a", "b");
        } catch (const lrstd::format_error&) {
            threw = true;
        }
        REQUIRE(threw);
    }

    {
        char c = 120;
        REQUIRE(format("{:6}", 42) == "    42");
        REQUIRE(format("{:6}", 'x') == "x     ");
        REQUIRE(format("{:*<6}", 'x') == "x*****");
        REQUIRE(format("{:*>6}", 'x') == "*****x");
        REQUIRE(format("{:*^6}", 'x') == "**x***");
        REQUIRE(format("{:*^{}}", 'x', 6) == "**x***");
        REQUIRE(format("{:6d}", c) == "   120");
        REQUIRE(format("{:6}", true) == "true  ");
    }

    {
          // clang-format off

        // TODO
        // double inf = std::numeric_limits<double>::infinity();
        // double nan = std::numeric_limits<double>::quiet_NaN();
        // REQUIRE(format("{0:},{0:+},{0:-},{0: }", 1) == "1,+1,1, 1");
        // REQUIRE(format("{0:},{0:+},{0:-},{0: }", -1) == "-1,-1,-1,-1");
        // REQUIRE(format("{0:},{0:+},{0:-},{0: }", inf) == "inf,+inf,inf, inf");
        // REQUIRE(format("{0:},{0:+},{0:-},{0: }", nan) == "nan,+nan,nan, nan");

          // clang-format on
    }

    {
        char c = 120;
        REQUIRE(format("{:+06d}", c) == "+00120");
        REQUIRE(format("{:#06x}", 0xa) == "0x000a");
        REQUIRE(format("{:<06}", -42) == "-42   ");
    }

    {
        REQUIRE(format("{}", 42) == "42");
        REQUIRE(format("{0:b} {0:d} {0:o} {0:x}", 42) == "101010 42 52 2a");
        REQUIRE(format("{0:#x} {0:#X}", 42) == "0x2a 0X2A");

        // clang-format off
        // TODO
        // REQUIRE(format("{:L}", 1234) == "1,234");  // (depending on the locale)
        // clang-format on
    }
    { REQUIRE(format("{}", red) == "red"); }
    { REQUIRE(format("{0:{1}}", S{42}, 10) == "xxxxxxxx42"); }
}
