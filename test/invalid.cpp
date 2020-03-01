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

TEMPLATE_TEST_CASE("invalid_int", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    CHECK_THROWS_AS(format(str("{:c}"), 500), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:s}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:a}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:A}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:e}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:f}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:F}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:g}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:G}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:n}"), 5), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:p}"), 5), lrstd::format_error);
}

TEMPLATE_TEST_CASE("invalid_char", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    TestType c = '5';
    CHECK_THROWS_AS(format(str("{:+}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:-}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{: }"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:#}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:+#}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:-#}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{: #}"), c), lrstd::format_error);

    CHECK_THROWS_AS(format(str("{:+c}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:-c}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{: c}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:#c}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:+#c}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:-#c}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{: #c}"), c), lrstd::format_error);

    CHECK_THROWS_AS(format(str("{:0}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:0c}"), c), lrstd::format_error);
    CHECK_NOTHROW(format(str("{:<0}"), c));
    CHECK_NOTHROW(format(str("{:<0c}"), c));

    CHECK_THROWS_AS(format(str("{:s}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:a}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:A}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:e}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:f}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:F}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:g}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:G}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:n}"), c), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:p}"), c), lrstd::format_error);
}