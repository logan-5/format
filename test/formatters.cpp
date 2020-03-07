#include "converter.hpp"
#include "format.hpp"

#include <string>
#include <string_view>

#include <catch.hpp>

TEMPLATE_TEST_CASE("formatters", "[formatters]", char, wchar_t) {
    using namespace std::literals;
    using lrstd::format;

    str_fn<TestType> str;

    {
        CHECK(format(str("")) == str(""));
        CHECK(format(str("hey")) == str("hey"));
        CHECK(format(str("{}"), str("")) == str(""));
        CHECK(format(str("he{}y"), str("")) == str("hey"));
    }

    {
        CHECK(format(str("{}"), str("hey")) == str("hey"));
        CHECK(format(str("{0: ^6}"), str("hey")) == str(" hey  "));
        CHECK(format(str("{:_>6}"), str("hey")) == str("___hey"));
    }
    {
        CHECK(format(str("{}"), str("hey"s)) == str("hey"));
        CHECK(format(str("{0: ^6}"), str("hey"s)) == str(" hey  "));
        CHECK(format(str("{:_>6}"), str("hey"s)) == str("___hey"));
    }
    {
        CHECK(format(str("{}"), str("hey"sv)) == str("hey"));
        CHECK(format(str("{0: ^6}"), str("hey"sv)) == str(" hey  "));
        CHECK(format(str("{:_>6}"), str("hey"sv)) == str("___hey"));
    }
    {
        CHECK(format(str("{:.5}"), str("strings can have precision")) ==
              str("strin"));
        CHECK(format(str("{:.5s}"), str("strings can have precision")) ==
              str("strin"));
        CHECK(format(str("{:.0}"), str("strings can have precision")) ==
              str(""));
    }
    {
        CHECK(format(str("{:}"), true) == str("true"));
        CHECK(format(str("{0: ^6}"), true) == str(" true "));
        CHECK(format(str("{:_>6}"), false) == str("_false"));
    }
    {
        CHECK(format(str("{:s}"), true) == str("true"));
        CHECK(format(str("{0: ^6s}"), true) == str(" true "));
        CHECK(format(str("{:_>6s}"), false) == str("_false"));
    }
    {
        CHECK(format(str("{:c}"), true) == str("\x1"));
        CHECK(format(str("{0: ^6c}"), true) == str("  \x1   "));
        CHECK(format(str("{:_>6c}"), false) ==
              str(std::string_view("_____\x0", 6)));
    }
    {
        CHECK(format(str("{:d}"), true) == str("1"));
        CHECK(format(str("{0: ^6d}"), true) == str("  1   "));
        CHECK(format(str("{:_>6d}"), false) == str("_____0"));
    }
    {
        CHECK(format(str("{}"), (char)123) == str("{"));
        CHECK(format(str("{:d}"), (char)123) == str("123"));
        CHECK(format(str("{}"), (unsigned char)123) == str("123"));
        CHECK(format(str("{}"), (short)12345) == str("12345"));
        CHECK(format(str("{}"), 12345) == str("12345"));
        CHECK(format(str("{}"), 12345u) == str("12345"));
        CHECK(format(str("{}"), 12345l) == str("12345"));
        CHECK(format(str("{}"), 12345lu) == str("12345"));
        CHECK(format(str("{}"), 12345ll) == str("12345"));
        CHECK(format(str("{}"), 12345llu) == str("12345"));
    }
    {
        CHECK(format(str("{:Ld}"), (char)123) == str("123"));
        CHECK(format(str("{:L}"), (unsigned char)123) == str("123"));
        CHECK(format(str("{:L}"), (short)123) == str("123"));
        CHECK(format(str("{:L}"), 123) == str("123"));
        CHECK(format(str("{:L}"), 123u) == str("123"));
        CHECK(format(str("{:L}"), 123l) == str("123"));
        CHECK(format(str("{:L}"), 123lu) == str("123"));
        CHECK(format(str("{:L}"), 123ll) == str("123"));
        CHECK(format(str("{:L}"), 123llu) == str("123"));
    }
    {
        std::locale::global(std::locale("en_US"));
        CHECK(format(str("{:L}"), 12345) == str("12,345"));
        CHECK(format(str("{:L}"), 12345u) == str("12,345"));
        CHECK(format(str("{:L}"), 12345l) == str("12,345"));
        CHECK(format(str("{:L}"), 12345lu) == str("12,345"));
        CHECK(format(str("{:L}"), 12345ll) == str("12,345"));
        CHECK(format(str("{:L}"), 12345llu) == str("12,345"));
    }
    {
        CHECK(format(str("{}"), nullptr) == str("0x0"));
        CHECK(format(str("{:p}"), nullptr) == str("0x0"));

        std::uintptr_t p = 0x12345678;
        CHECK(format(str("{}"), reinterpret_cast<void*>(p)) ==
              str("0x12345678"));
        CHECK(format(str("{:p}"), reinterpret_cast<void*>(p)) ==
              str("0x12345678"));
        CHECK(format(str("{}"), reinterpret_cast<const void*>(p)) ==
              str("0x12345678"));
        CHECK(format(str("{:p}"), reinterpret_cast<const void*>(p)) ==
              str("0x12345678"));

        CHECK_THROWS_AS(format(str("{:d}"), reinterpret_cast<void*>(p)),
                        lrstd::format_error);
    }
}

// on some versions of libc++ to_chars (that ship with xcode anyway), a
// bunch of zeros are prepended to longs
TEST_CASE("weird_to_chars_bug", "[formatters][!mayfail]") {
    CHECK(lrstd::format("{}", 0x12345678) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678u) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678l) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678ul) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678ll) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678ull) == "305419896");
}

TEMPLATE_TEST_CASE("escaping", "[formatters]", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    CHECK(format(str("{{")) == str("{"));
    CHECK(format(str("}}")) == str("}"));
    CHECK(format(str("{}"), str("{{")) == str("{{"));
    CHECK(format(str("{}"), str("}}")) == str("}}"));
    CHECK(format(str("{{{{")) == str("{{"));
    CHECK(format(str("}}}}")) == str("}}"));

    CHECK(format(str("{}{{"), 5) == str("5{"));
    CHECK(format(str("{}}}"), 5) == str("5}"));
    CHECK(format(str("{}a{{"), 5) == str("5a{"));
    CHECK(format(str("{}a}}"), 5) == str("5a}"));
    CHECK(format(str("aa{}a{{aa"), 5) == str("aa5a{aa"));
    CHECK(format(str("aa{}a}}aa"), 5) == str("aa5a}aa"));
    CHECK(format(str("aa{{{}a{{aa"), 5) == str("aa{5a{aa"));
    CHECK(format(str("aa}}{}a}}aa"), 5) == str("aa}5a}aa"));
    CHECK(format(str("aa{{{}a}}aa"), 5) == str("aa{5a}aa"));
    CHECK(format(str("aa}}{}a{{aa"), 5) == str("aa}5a{aa"));
}

TEMPLATE_TEST_CASE("dynamic_width", "[formatters]", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    CHECK(format(str("{:{}}"), 1, 2) == str(" 1"));
    CHECK(format(str("{0:{1}}"), 1, 2) == str(" 1"));
    CHECK(format(str("{1:{1}}"), 1, 2) == str(" 2"));
    CHECK(format(str("{1:{0}}"), 1, 2) == str("2"));

    CHECK_THROWS_AS(format(str("{0:{}}"), 1, 2), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:{1}}"), 1, 2), lrstd::format_error);
}
