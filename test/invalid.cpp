#include "format.hpp"

#include "converter.hpp"

#include <catch.hpp>

#include <string>

TEMPLATE_TEST_CASE("invalid", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    for (auto& pattern : {str("{:0}"), str("{:500}"), str("{}"), str("{:}")}) {
        CHECK_THROWS_AS(format(pattern), lrstd::format_error);
        CHECK(format(str("{}"), pattern) == pattern);
    }
}

TEMPLATE_TEST_CASE("type_parsing_bug", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    CHECK_THROWS_AS(format(str("{:xd}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:dx}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:aAbBcdeEfFgGnopsxX}"), 5),
                    lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:aAbBcdeEfFgGnop}"), 5), lrstd::format_error);
}