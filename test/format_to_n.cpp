#include "format.hpp"

#include <catch.hpp>

#include <array>
#include <string>
#include <string_view>
#include <vector>

template <class Char>
struct SV {
    template <std::size_t N>
    std::basic_string_view<Char> operator()(
          const std::array<Char, N>& arr) const {
        return {arr.data()};
    }
    template <class C>
    std::basic_string_view<Char> operator()(const C& c) const {
        return {std::data(c), std::size(c)};
    }
};

TEST_CASE("format_to_n", "") {
    using namespace std::string_view_literals;
    using namespace lrstd;

    SV<char> sv;

    {
        std::array<char, 64> chars{};
        {
            auto result =
                  format_to_n(chars.begin(), 11, "unavoidable paradigm");
            REQUIRE(result.size == 20);
            REQUIRE(result.out == std::next(chars.begin(), 11));
            REQUIRE(sv(chars) == "unavoidable");
        }
        {
            auto result = format_to_n(chars.begin(), 16,
                                      "the system is {1} free from {0}",
                                      "error", "now");
            REQUIRE(result.size == 33);
            REQUIRE(result.out == std::next(chars.begin(), 16));
            REQUIRE(sv(chars) == "the system is no");
        }
    }
    {
        std::vector<char> chars;
        {
            auto result = format_to_n(std::back_inserter(chars), 10000000,
                                      "the {}", "system");
            REQUIRE(result.size == 10);
            REQUIRE(sv(chars) == "the system");
        }
        {
            auto result = format_to_n(std::back_inserter(chars), 0,
                                      " revokes our {} form", "viral");
            REQUIRE(result.size == 23);
            REQUIRE(sv(chars) == "the system");
        }
    }
    {
        std::string chars;
        auto result = format_to_n(std::back_inserter(chars), 20, "{:*^20}", "");
        REQUIRE(chars == "********************");
        REQUIRE(result.size == 20);
        chars.clear();
        result = format_to_n(std::back_inserter(chars), 10, "{:*>20}", "");
        REQUIRE(chars == "**********");
        REQUIRE(result.size == 20);
    }
    {
        std::string chars;
        auto result = format_to_n(std::back_inserter(chars), 200,
                                  "{0:_>5}{0:-^3}{0:_<5}", 'a');
        REQUIRE(chars == "____a-a-a____");
        REQUIRE(result.size == 13);
        chars.clear();
        result = format_to_n(std::back_inserter(chars), 8,
                             "{0:_>5}{0:-^3}{0:_<5}", 'a');
        REQUIRE(chars == "____a-a-");
        REQUIRE(result.size == 13);
    }
}

TEST_CASE("format_to_n_wide", "") {
    using namespace std::string_view_literals;
    using namespace lrstd;

    SV<wchar_t> sv;

    {
        std::array<wchar_t, 64> chars{};
        {
            auto result =
                  format_to_n(chars.begin(), 11, L"unavoidable paradigm");
            REQUIRE(result.size == 20);
            REQUIRE(result.out == std::next(chars.begin(), 11));
            REQUIRE(sv(chars) == L"unavoidable");
        }
        {
            auto result = format_to_n(chars.begin(), 16,
                                      L"the system is {1} free from {0}",
                                      L"error", L"now");
            REQUIRE(result.size == 33);
            REQUIRE(result.out == std::next(chars.begin(), 16));
            REQUIRE(sv(chars) == L"the system is no");
        }
    }
    {
        std::vector<wchar_t> chars;
        {
            auto result = format_to_n(std::back_inserter(chars), 10000000,
                                      L"the {}", L"system");
            REQUIRE(result.size == 10);
            REQUIRE(sv(chars) == L"the system");
        }
        {
            auto result = format_to_n(std::back_inserter(chars), 0,
                                      L" revokes our {} form", L"viral");
            REQUIRE(result.size == 23);
            REQUIRE(sv(chars) == L"the system");
        }
    }
    {
        std::wstring chars;
        auto result =
              format_to_n(std::back_inserter(chars), 20, L"{:*^20}", L"");
        REQUIRE(chars == L"********************");
        REQUIRE(result.size == 20);
        chars.clear();
        result = format_to_n(std::back_inserter(chars), 10, L"{:*>20}", L"");
        REQUIRE(chars == L"**********");
        REQUIRE(result.size == 20);
    }
    {
        std::wstring chars;
        auto result = format_to_n(std::back_inserter(chars), 200,
                                  L"{0:_>5}{0:-^3}{0:_<5}", L'a');
        REQUIRE(chars == L"____a-a-a____");
        REQUIRE(result.size == 13);
        chars.clear();
        result = format_to_n(std::back_inserter(chars), 8,
                             L"{0:_>5}{0:-^3}{0:_<5}", L'a');
        REQUIRE(chars == L"____a-a-");
        REQUIRE(result.size == 13);
    }
}
