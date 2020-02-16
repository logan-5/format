#include "format.hpp"

#include <catch.hpp>

using lrstd::detail::representable_as_char;

TEST_CASE("representable_as_char", "[char]") {
    CHECK(representable_as_char<char>('a'));
    CHECK(representable_as_char<char>(123));
    CHECK(representable_as_char<char>(123u));

    CHECK_FALSE(representable_as_char<char>(std::numeric_limits<int>::max()));
    CHECK_FALSE(representable_as_char<char>(
          std::numeric_limits<unsigned int>::max()));
    CHECK_FALSE(representable_as_char<char>(std::numeric_limits<int>::min()));
    CHECK(representable_as_char<char>(
          std::numeric_limits<unsigned int>::min()));

    CHECK(representable_as_char<unsigned char>(128));
    CHECK_FALSE(representable_as_char<signed char>(128));
    CHECK(representable_as_char<signed char>(-1));
    CHECK_FALSE(representable_as_char<unsigned char>(-1));
    CHECK_FALSE(representable_as_char<signed char>((unsigned char)-1));
    CHECK_FALSE(representable_as_char<signed char>((unsigned char)128));
    CHECK_FALSE(representable_as_char<unsigned char>((signed char)-1));
}

TEST_CASE("representable_as_wchar", "[char][wchar]") {
    CHECK(representable_as_char<wchar_t>(0));
    CHECK(representable_as_char<wchar_t>(1));
    CHECK(representable_as_char<wchar_t>('0'));
    CHECK(representable_as_char<wchar_t>('1'));
    CHECK(representable_as_char<wchar_t>('\x0'));
    CHECK(representable_as_char<wchar_t>('\x1'));
    CHECK(representable_as_char<wchar_t>(u'\x0'));
    CHECK(representable_as_char<wchar_t>(u'\x1'));
    CHECK(representable_as_char<wchar_t>(U'\x0'));
    CHECK(representable_as_char<wchar_t>(U'\x1'));
    CHECK_FALSE(representable_as_char<wchar_t>(
          std::numeric_limits<wchar_t>::max() + 1ll));
    CHECK_FALSE(representable_as_char<wchar_t>(
          std::numeric_limits<wchar_t>::min() - 1ll));
}
