#include "format.hpp"

#include <catch.hpp>

TEST_CASE("representable_as_char", "") {
    using lrstd::detail::representable_as_char;

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
