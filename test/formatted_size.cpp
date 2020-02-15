#include "format.hpp"

#include <string>
#include <string_view>

#include <catch.hpp>

namespace {
enum color { red, green, blue };
const char* color_names[] = {"red", "green", "blue"};
}  // namespace
template <>
struct lrstd::formatter<color> : lrstd::formatter<const char*> {
    // TODO: the implementation of formatted_size uses a special iterator, which
    // requires this to be templated on the output iterator. is that conforming?
    template <class Out>
    auto format(color c, basic_format_context<Out, char>& ctx) {
        return formatter<const char*>::format(color_names[c], ctx);
    }
};
namespace {
struct S {
    int value;
};
}  // namespace
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
    template <class Out>
    auto format(S s, basic_format_context<Out, char>& ctx) {
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

TEST_CASE("formatted_size", "") {
    using namespace std::literals;
    using lrstd::formatted_size;
    {
        CHECK(formatted_size("{}", "hey") == 3);
        CHECK(formatted_size("{0: ^6}", "hey") == 6);
        CHECK(formatted_size("{:_>6}", "hey") == 6);
    }
    {
        CHECK(formatted_size("{:s}", true) == 4);
        CHECK(formatted_size("{:s}", false) == 5);
        CHECK(formatted_size("{0: ^6s}", true) == 6);
        CHECK(formatted_size("{:_>6s}", false) == 6);
    }

    CHECK(formatted_size("{0}-{{", 8) == 3);

    CHECK(formatted_size("{} to {}", "a", "b") == 6);
    CHECK(formatted_size("{1} to {0}", "a", "b") == 6);

    {
        char c = 120;
        CHECK(formatted_size("{:6}", 42) == 6);
        CHECK(formatted_size("{:6}", 'x') == 6);
        CHECK(formatted_size("{:*<6}", 'x') == 6);
        CHECK(formatted_size("{:*>6}", 'x') == 6);
        CHECK(formatted_size("{:*^6}", 'x') == 6);
        CHECK(formatted_size("{:*^{}}", 'x', 6) == 6);
        CHECK(formatted_size("{:6d}", c) == 6);
        CHECK(formatted_size("{:6}", true) == 6);
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
        char c = 120;
        CHECK(formatted_size("{:+06d}", c) == 6);
        CHECK(formatted_size("{:#06x}", 0xa) == 6);
        CHECK(formatted_size("{:<06}", -42) == 6);
    }

    {
        CHECK(formatted_size("{}", 42) == 2);
        CHECK(formatted_size("{0:b} {0:d} {0:o} {0:x}", 42) == 15);
        CHECK(formatted_size("{0:#x} {0:#X}", 42) == 9);

        // clang-format off
        // TODO
        // CHECK(format("{:L}", 1234) == "1,234");  // (depending on the locale)
        // clang-format on
    }
    { CHECK(formatted_size("{}", red) == 3); }
    { CHECK(formatted_size("{0:{1}}", S{42}, 10) == 10); }
}

TEST_CASE("formatted_size_wide", "") {
    using namespace std::literals;
    using lrstd::formatted_size;
    {
        CHECK(formatted_size(L"{}", L"hey") == 3);
        CHECK(formatted_size(L"{0: ^6}", L"hey") == 6);
        CHECK(formatted_size(L"{:_>6}", L"hey") == 6);
    }
    {
        CHECK(formatted_size(L"{:s}", true) == 4);
        CHECK(formatted_size(L"{:s}", false) == 5);
        CHECK(formatted_size(L"{0: ^6s}", true) == 6);
        CHECK(formatted_size(L"{:_>6s}", false) == 6);
    }

    CHECK(formatted_size(L"{0}-{{", 8) == 3);

    CHECK(formatted_size(L"{} to {}", L"a", L"b") == 6);
    CHECK(formatted_size(L"{1} to {0}", L"a", L"b") == 6);

    {
        char c = 120;
        CHECK(formatted_size(L"{:6}", 42) == 6);
        CHECK(formatted_size(L"{:6}", 'x') == 6);
        CHECK(formatted_size(L"{:*<6}", 'x') == 6);
        CHECK(formatted_size(L"{:*>6}", 'x') == 6);
        CHECK(formatted_size(L"{:*^6}", 'x') == 6);
        CHECK(formatted_size(L"{:*^{}}", 'x', 6) == 6);
        CHECK(formatted_size(L"{:6d}", c) == 6);
        CHECK(formatted_size(L"{:6}", true) == 6);
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
        char c = 120;
        CHECK(formatted_size(L"{:+06d}", c) == 6);
        CHECK(formatted_size(L"{:#06x}", 0xa) == 6);
        CHECK(formatted_size(L"{:<06}", -42) == 6);
    }

    {
        CHECK(formatted_size(L"{}", 42) == 2);
        CHECK(formatted_size(L"{0:b} {0:d} {0:o} {0:x}", 42) == 15);
        CHECK(formatted_size(L"{0:#x} {0:#X}", 42) == 9);

        // clang-format off
        // TODO
        // CHECK(format("{:L}", 1234) == "1,234");  // (depending on the locale)
        // clang-format on
    }
}
