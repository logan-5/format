#include "format.hpp"

#include <string>
#include <string_view>

#include <catch.hpp>

TEST_CASE("formatters", "[formatters]") {
    using namespace std::literals;
    using lrstd::format;
    {
        CHECK(format("{}", "hey") == "hey");
        CHECK(format("{0: ^6}", "hey") == " hey  ");
        CHECK(format("{:_>6}", "hey") == "___hey");
    }
    {
        CHECK(format("{}", "hey"s) == "hey");
        CHECK(format("{0: ^6}", "hey"s) == " hey  ");
        CHECK(format("{:_>6}", "hey"s) == "___hey");
    }
    {
        CHECK(format("{}", "hey"sv) == "hey");
        CHECK(format("{0: ^6}", "hey"sv) == " hey  ");
        CHECK(format("{:_>6}", "hey"sv) == "___hey");
    }
    {
        CHECK(format("{:}", true) == "true");
        CHECK(format("{0: ^6}", true) == " true ");
        CHECK(format("{:_>6}", false) == "_false");
    }
    {
        CHECK(format("{:s}", true) == "true");
        CHECK(format("{0: ^6s}", true) == " true ");
        CHECK(format("{:_>6s}", false) == "_false");
    }
    {
        CHECK(format("{:c}", true) == "\x1");
        CHECK(format("{0: ^6c}", true) == "  \x1   ");
        CHECK(format("{:_>6c}", false) == std::string_view("_____\x0", 6));
    }
    {
        CHECK(format("{:d}", true) == "1");
        CHECK(format("{0: ^6d}", true) == "  1   ");
        CHECK(format("{:_>6d}", false) == "_____0");
    }
    {
        CHECK(format("{}", (char)123) == "{");
        CHECK(format("{:d}", (char)123) == "123");
        CHECK(format("{}", (short)12345) == "12345");
        CHECK(format("{}", 12345) == "12345");
        CHECK(format("{}", 12345u) == "12345");
        CHECK(format("{}", 12345l) == "12345");
        CHECK(format("{}", 12345lu) == "12345");
        CHECK(format("{}", 12345ll) == "12345");
        CHECK(format("{}", 12345llu) == "12345");
    }
    {
        CHECK(format("{}", nullptr) == "0x0");
        CHECK(format("{:p}", nullptr) == "0x0");

        std::uintptr_t p = 0x12345678;
        CHECK(format("{}", reinterpret_cast<void*>(p)) == "0x12345678");
        CHECK(format("{:p}", reinterpret_cast<void*>(p)) == "0x12345678");
        CHECK(format("{}", reinterpret_cast<const void*>(p)) == "0x12345678");
        CHECK(format("{:p}", reinterpret_cast<const void*>(p)) == "0x12345678");
        {
            bool threw = false;
            try {
                format("{:d}", reinterpret_cast<void*>(p));
            } catch (lrstd::format_error&) {
                threw = true;
            }
            CHECK(threw);
        }
    }
}

// on some versions of libc++ to_chars (that ship with xcode anyway), a
// bunch of zeros are prepended to longs
TEST_CASE("weird_to_chars_bug", "[formatters][!mayfail]") {
    CHECK(lrstd::format("{}", 0x12345678l) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678ul) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678ll) == "305419896");
    CHECK(lrstd::format("{}", 0x12345678ull) == "305419896");
}
