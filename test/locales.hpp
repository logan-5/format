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

template <template <class> class Facet>
struct locale_with_facet {
    std::locale operator()() const {
        std::locale loc(std::locale::classic(), new Facet<char>);
        return std::locale(loc, new Facet<wchar_t>);
    }
};

using en_US_locale = locale_with_facet<en_US_facet>;
using en_US_funky_locale = locale_with_facet<en_US_funky_facet>;