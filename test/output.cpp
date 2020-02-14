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

TEST_CASE("output", "") {
    using namespace std::string_view_literals;
    using namespace lrstd;

    SV<char> sv;

    {
        std::array<char, 64> chars{};
        REQUIRE(format_to(chars.begin(), "unavoidable paradigm") ==
                std::next(chars.begin(), 20));
        REQUIRE(sv(chars) == "unavoidable paradigm");
        REQUIRE(format_to(chars.begin(), "the system is {1} free from {0}",
                          "error", "now") == std::next(chars.begin(), 33));
        REQUIRE(sv(chars) == "the system is now free from error");
    }
    {
        std::vector<char> chars;
        format_to(std::back_inserter(chars), "the {}", "system");
        REQUIRE(sv(chars) == "the system");
        format_to(std::back_inserter(chars), " revokes our {} form", "viral");
        REQUIRE(sv(chars) == "the system revokes our viral form");
    }
    {
        std::string chars;
        format_to(std::back_inserter(chars), "the {}", "radiant sun");
        REQUIRE(chars == "the radiant sun");
        format_to(std::back_inserter(chars), " engraves its {}", "name");
        REQUIRE(chars == "the radiant sun engraves its name");
    }
    {
        std::string chars;
        std::string fmt = "we cannot see past the {} shroud";
        format_to(std::back_inserter(chars), fmt, "solar");
        REQUIRE(chars == "we cannot see past the solar shroud");
        chars.clear();
        format_to(std::back_inserter(chars), fmt, fmt);
        REQUIRE(
              chars ==
              "we cannot see past the we cannot see past the {} shroud shroud");
    }
    {
        std::string chars =
              "___ transport will be outfitted for a journey spanning future "
              "generations";
        format_to(chars.begin(), "the {}", sv(chars).substr(4));
        REQUIRE(chars ==
                "the transport will be outfitted for a journey spanning future "
                "generations");
    }
}

TEST_CASE("output_wide", "") {
    using namespace std::string_view_literals;
    using namespace lrstd;

    SV<wchar_t> sv;

    {
        std::array<wchar_t, 64> chars{};
        REQUIRE(format_to(chars.begin(), L"unavoidable paradigm") ==
                std::next(chars.begin(), 20));
        REQUIRE(sv(chars) == L"unavoidable paradigm");
        REQUIRE(format_to(chars.begin(), L"the system is {1} free from {0}",
                          L"error", L"now") == std::next(chars.begin(), 33));
        REQUIRE(sv(chars) == L"the system is now free from error");
    }
    {
        std::vector<wchar_t> chars;
        format_to(std::back_inserter(chars), L"the {}", L"system");
        REQUIRE(sv(chars) == L"the system");
        format_to(std::back_inserter(chars), L" revokes our {} form", L"viral");
        REQUIRE(sv(chars) == L"the system revokes our viral form");
    }
    {
        std::wstring chars;
        format_to(std::back_inserter(chars), L"the {}", L"radiant sun");
        REQUIRE(chars == L"the radiant sun");
        format_to(std::back_inserter(chars), L" engraves its {}", L"name");
        REQUIRE(chars == L"the radiant sun engraves its name");
    }
    {
        std::wstring chars;
        std::wstring fmt = L"we cannot see past the {} shroud";
        format_to(std::back_inserter(chars), fmt, L"solar");
        REQUIRE(chars == L"we cannot see past the solar shroud");
        chars.clear();
        format_to(std::back_inserter(chars), fmt, fmt);
        REQUIRE(chars ==
                L"we cannot see past the we cannot see past the {} shroud "
                L"shroud");
    }
    {
        std::wstring chars =
              L"___ transport will be outfitted for a journey spanning future "
              L"generations";
        format_to(chars.begin(), L"the {}", sv(chars).substr(4));
        REQUIRE(
              chars ==
              L"the transport will be outfitted for a journey spanning future "
              L"generations");
    }
}
