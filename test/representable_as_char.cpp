#include "format.hpp"

#include <catch.hpp>

TEST_CASE("representable_as_char", "") {
    using lrstd::detail::representable_as_char;

    REQUIRE(representable_as_char<char>('a'));
    REQUIRE(representable_as_char<char>(123));
    REQUIRE(representable_as_char<char>(123u));

    REQUIRE(!representable_as_char<char>(std::numeric_limits<int>::max()));
    REQUIRE(!representable_as_char<char>(
          std::numeric_limits<unsigned int>::max()));
    REQUIRE(!representable_as_char<char>(std::numeric_limits<int>::min()));
    REQUIRE(representable_as_char<char>(
          std::numeric_limits<unsigned int>::min()));

    REQUIRE(representable_as_char<unsigned char>(128));
    REQUIRE(!representable_as_char<signed char>(128));
    REQUIRE(representable_as_char<signed char>(-1));
    REQUIRE(!representable_as_char<unsigned char>(-1));
    REQUIRE(!representable_as_char<signed char>((unsigned char)-1));
    REQUIRE(!representable_as_char<signed char>((unsigned char)128));
    REQUIRE(!representable_as_char<unsigned char>((signed char)-1));
}
