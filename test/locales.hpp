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

struct en_US_locale {
    inline std::locale operator()() const {
        std::locale loc(std::locale(), new en_US_facet<char>);
        loc.combine<en_US_facet<wchar_t>>(
              std::locale(std::locale(), new en_US_facet<wchar_t>));
        return loc;
    }
};

struct en_US_funky_locale {
    inline std::locale operator()() const {
        std::locale loc(std::locale(), new en_US_funky_facet<char>);
        loc.combine<en_US_funky_facet<wchar_t>>(
              std::locale(std::locale(), new en_US_funky_facet<wchar_t>));
        return loc;
    }
};