#pragma once

#include <locale>

template <class CharT>
struct en_US_facet : std::numpunct<CharT> {
    CharT do_thousands_sep() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
};

template <class CharT>
struct en_US_funky_facet : std::numpunct<CharT> {
    CharT do_thousands_sep() const override { return ','; }
    std::string do_grouping() const override { return "\3\3\3\3\3"; }
};

template <class CharT>
struct embedded_null_facet : std::numpunct<CharT> {
    CharT do_thousands_sep() const override { return '%'; }
    std::string do_grouping() const override {
        return std::string("\2\1\0\1\2", 5);
    }
};

template <class CharT>
struct embedded_char_max_facet : std::numpunct<CharT> {
    CharT do_thousands_sep() const override { return '%'; }
    std::string do_grouping() const override {
        std::string s = "\2\1";
        s += CHAR_MAX;
        s += "\1\2";
        return s;
    }
};

template <class CharT>
struct embedded_negative_char_facet : std::numpunct<CharT> {
    static_assert(std::is_signed_v<char>);

    CharT do_thousands_sep() const override { return '%'; }
    std::string do_grouping() const override {
        std::string s = "\2\1";
        s += -2;
        s += "\1\2";
        return s;
    }
};

template <template <class> class Facet>
struct locale_with_facet {
    std::locale operator()() const {
        std::locale loc(std::locale::classic(), new Facet<char>);
        return std::locale(loc, new Facet<wchar_t>);
    }
};

using en_US_locale = locale_with_facet<en_US_facet>;
using en_US_funky_locale = locale_with_facet<en_US_funky_facet>;
using embedded_null_locale = locale_with_facet<embedded_null_facet>;
using embedded_char_max_locale = locale_with_facet<embedded_char_max_facet>;
using embedded_negative_char_locale =
      locale_with_facet<embedded_char_max_facet>;
