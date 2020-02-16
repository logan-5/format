#include "converter.hpp"
#include "format.hpp"

#include <catch.hpp>

#include <array>
#include <string>
#include <string_view>
#include <vector>

TEMPLATE_TEST_CASE("output_to_array", "", char, wchar_t) {
    using namespace lrstd;

    SV<TestType> sv;
    str_fn<TestType> str;
    std::array<TestType, 64> chars{};
    CHECK(format_to(chars.begin(), str("unavoidable paradigm")) ==
          std::next(chars.begin(), 20));
    CHECK(sv(chars) == str("unavoidable paradigm"));
    CHECK(format_to(chars.begin(), str("the system is {1} free from {0}"),
                    str("error"), str("now")) == std::next(chars.begin(), 33));
    CHECK(sv(chars) == str("the system is now free from error"));
}

TEMPLATE_TEST_CASE("output", "", char, wchar_t) {
    using namespace std::string_view_literals;
    using namespace lrstd;

    SV<TestType> sv;
    str_fn<TestType> str;

    {
        std::vector<TestType> chars;
        format_to(std::back_inserter(chars), str("the {}"), str("system"));
        CHECK(sv(chars) == str("the system"));
        format_to(std::back_inserter(chars), str(" revokes our {} form"),
                  str("viral"));
        CHECK(sv(chars) == str("the system revokes our viral form"));
    }
    {
        std::basic_string<TestType> chars;
        format_to(std::back_inserter(chars), str("the {}"), str("radiant sun"));
        CHECK(chars == str("the radiant sun"));
        format_to(std::back_inserter(chars), str(" engraves its {}"),
                  str("name"));
        CHECK(chars == str("the radiant sun engraves its name"));
    }
    {
        std::basic_string<TestType> chars;
        std::basic_string<TestType> fmt =
              str("we cannot see past the {} shroud");
        format_to(std::back_inserter(chars), fmt, str("solar"));
        CHECK(chars == str("we cannot see past the solar shroud"));
        chars.clear();
        format_to(std::back_inserter(chars), fmt, fmt);
        CHECK(chars == str("we cannot see past the we cannot see past the {} "
                           "shroud shroud"));
    }
    {
        std::basic_string<TestType> chars = str(
              "___ transport will be outfitted for a journey spanning future "
              "generations");
        format_to(chars.begin(), str("the {}"), sv(chars).substr(4));
        CHECK(chars == str("the transport will be outfitted for a journey "
                           "spanning future "
                           "generations"));
    }
}
