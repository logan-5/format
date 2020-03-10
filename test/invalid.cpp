#include "format.hpp"

#include "converter.hpp"

#include <catch.hpp>

#include <string>

TEMPLATE_TEST_CASE("invalid", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    for (auto& pattern : {
               str("{}"),
               str("{:}"),
               str("{0}"),
               str("{500}"),
               str("{0:}"),
               str("{500:}"),
               str("{0:0}"),
               str("{500:500}"),
               str("{:0}"),
               str("{:500}"),
         }) {
        CHECK_THROWS_AS(format(pattern), lrstd::format_error);
        CHECK(format(str("{}"), pattern) == pattern);
    }
}

TEMPLATE_TEST_CASE("parse_errors", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    for (auto& pattern : {
               str("}{"),          str("{:.}"),        str("{:00}"),
               str("{:x0}"),       str("{:x#}"),       str("{:LL}"),
               str("{"),           str("}"),           str("{}}"),
               str("cold{brew"),   str("cold}brew"),   str("coldbrew}"),
               str("coldbrew{"),   str("{coldbrew"),   str("}coldbrew"),
               str("{}coldbrew{"), str("{}coldbrew}"), str("{:{{}"),
               str("{x}"),         str("{qwerty}"),    str("{10x}"),
               str("{x10}"),       str("{x10x}"),      str("{4%*}"),
               str("{x:}"),        str("{qwerty:}"),   str("{10x:}"),
               str("{x10:}"),      str("{x10x:}"),     str("{4%*:}"),
         }) {
        CHECK_THROWS_AS(format(pattern, 5), lrstd::format_error);
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

TEMPLATE_TEST_CASE("invalid_dynamic_width_precision", "", char, wchar_t) {
    using lrstd::format;
    str_fn<TestType> str;

    CHECK(format(str("{:{}}"), 5, 6) == str("     5"));
    CHECK_THROWS_AS(format(str("{:{}}"), 5, str("hello")), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{:{}}"), 5, -6), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{0:{1}}"), 5, str("hello")),
                    lrstd::format_error);
    CHECK_THROWS_AS(format(str("{0:{1}}"), 5, -6), lrstd::format_error);
    CHECK_THROWS_AS(format(str("{1:{0}}"), -5, str("hello")),
                    lrstd::format_error);
    CHECK_THROWS_AS(format(str("{1:{0}}"), str("5"), -6), lrstd::format_error);
}

#define CHECK_TYPE_VERIFIER(FMT, ARG)                                  \
    do {                                                               \
        str_fn<charT> str;                                             \
        auto fmt = str(FMT);                                           \
        CHECK_THROWS_AS(lrstd::format(fmt, ARG), lrstd::format_error); \
    } while (false)

TEMPLATE_TEST_CASE("invalid_int", "", char, wchar_t) {
    using lrstd::format;
    using charT = TestType;
    CHECK_TYPE_VERIFIER("{:.1}", 5);
    CHECK_TYPE_VERIFIER("{:c}", 500);
    CHECK_TYPE_VERIFIER("{:s}", 5);
    CHECK_TYPE_VERIFIER("{:a}", 5);
    CHECK_TYPE_VERIFIER("{:A}", 5);
    CHECK_TYPE_VERIFIER("{:e}", 5);
    CHECK_TYPE_VERIFIER("{:f}", 5);
    CHECK_TYPE_VERIFIER("{:F}", 5);
    CHECK_TYPE_VERIFIER("{:g}", 5);
    CHECK_TYPE_VERIFIER("{:G}", 5);
    CHECK_TYPE_VERIFIER("{:n}", 5);
    CHECK_TYPE_VERIFIER("{:p}", 5);
}

TEMPLATE_TEST_CASE("invalid_nonarithmetic",
                   "",
                   (std::tuple<char, char>),
                   (std::tuple<wchar_t, char>),
                   (std::tuple<char, bool>),
                   (std::tuple<wchar_t, bool>),
                   (std::tuple<char, std::string>),
                   (std::tuple<wchar_t, std::wstring>),
                   (std::tuple<char, std::string_view>),
                   (std::tuple<wchar_t, std::wstring_view>),
                   (std::tuple<char, void*>),
                   (std::tuple<wchar_t, void*>)) {
    using lrstd::format;

    using charT = std::tuple_element_t<0, TestType>;
    using FormatteeType = std::tuple_element_t<1, TestType>;

    str_fn<charT> str;

    FormatteeType c = {};

    CHECK_TYPE_VERIFIER("{:+}", c);
    CHECK_TYPE_VERIFIER("{:-}", c);
    CHECK_TYPE_VERIFIER("{: }", c);
    CHECK_TYPE_VERIFIER("{:#}", c);
    CHECK_TYPE_VERIFIER("{:+#}", c);
    CHECK_TYPE_VERIFIER("{:-#}", c);
    CHECK_TYPE_VERIFIER("{: #}", c);

    CHECK_TYPE_VERIFIER("{:0}", c);
    CHECK_NOTHROW(format(str("{:<0}"), c));

    CHECK_TYPE_VERIFIER("{:a}", c);
    CHECK_TYPE_VERIFIER("{:A}", c);
    CHECK_TYPE_VERIFIER("{:e}", c);
    CHECK_TYPE_VERIFIER("{:f}", c);
    CHECK_TYPE_VERIFIER("{:F}", c);
    CHECK_TYPE_VERIFIER("{:g}", c);
    CHECK_TYPE_VERIFIER("{:G}", c);
    CHECK_TYPE_VERIFIER("{:n}", c);
}

TEMPLATE_TEST_CASE("invalid_nonarthmetic_nongeneric", "", char, wchar_t) {
    using lrstd::format;
    using charT = TestType;
    str_fn<charT> str;
    CHECK_TYPE_VERIFIER("{:s}", str('x'));

    CHECK_TYPE_VERIFIER("{:.1}", str('x'));
    CHECK_TYPE_VERIFIER("{:.1}", false);
    CHECK_TYPE_VERIFIER("{:p}", str('x'));
    CHECK_TYPE_VERIFIER("{:p}", false);

    CHECK_TYPE_VERIFIER("{:c}", str(""));
    CHECK_TYPE_VERIFIER("{:p}", str(""));

    CHECK_TYPE_VERIFIER("{:c}", (void*)nullptr);
    CHECK_TYPE_VERIFIER("{:s}", (void*)nullptr);

    CHECK_TYPE_VERIFIER("{:L}", str('x'));
    CHECK_TYPE_VERIFIER("{:L}", str("x"));
    CHECK_TYPE_VERIFIER("{:L}", (void*)nullptr);
    CHECK_NOTHROW(format(str("{:L}"), false));
}
