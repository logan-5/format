#include "converter.hpp"
#include "format.hpp"
#include "user_defined.hpp"

#include <catch.hpp>

#include <array>
#include <string>
#include <string_view>
#include <vector>

TEMPLATE_TEST_CASE("format_to_n", "", char, wchar_t) {
    using namespace std::string_view_literals;
    using namespace lrstd;

    str_fn<TestType> str;
    SV<TestType> sv;

    {
        std::array<TestType, 64> chars{};
        {
            auto result =
                  format_to_n(chars.begin(), 11, str("unavoidable paradigm"));
            CHECK(result.size == 20);
            CHECK(result.out == std::next(chars.begin(), 11));
            CHECK(sv(chars) == str("unavoidable"));
        }
        {
            auto result = format_to_n(chars.begin(), 16,
                                      str("the system is {1} free from {0}"),
                                      str("error"), str("now"));
            CHECK(result.size == 33);
            CHECK(result.out == std::next(chars.begin(), 16));
            CHECK(sv(chars) == str("the system is no"));
        }
    }
    {
        std::vector<TestType> chars;
        {
            auto result = format_to_n(std::back_inserter(chars), 10000000,
                                      str("the {}"), str("system"));
            CHECK(result.size == 10);
            CHECK(sv(chars) == str("the system"));
        }
        {
            auto result =
                  format_to_n(std::back_inserter(chars), 0,
                              str(" revokes our {} form"), str("viral"));
            CHECK(result.size == 23);
            CHECK(sv(chars) == str("the system"));
        }
    }
    {
        std::basic_string<TestType> chars;
        auto result = format_to_n(std::back_inserter(chars), 20, str("{:*^20}"),
                                  str(""));
        CHECK(chars == str("********************"));
        CHECK(result.size == 20);
        chars.clear();
        result = format_to_n(std::back_inserter(chars), 10, str("{:*>20}"),
                             str(""));
        CHECK(chars == str("**********"));
        CHECK(result.size == 20);
    }
    {
        std::basic_string<TestType> chars;
        auto result = format_to_n(std::back_inserter(chars), 200,
                                  str("{0:_>5}{0:-^3}{0:_<5}"), str('a'));
        CHECK(chars == str("____a-a-a____"));
        CHECK(result.size == 13);
        chars.clear();
        result = format_to_n(std::back_inserter(chars), 8,
                             str("{0:_>5}{0:-^3}{0:_<5}"), str('a'));
        CHECK(chars == str("____a-a-"));
        CHECK(result.size == 13);
    }
}
