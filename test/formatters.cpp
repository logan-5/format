#include "format.hpp"

#include <string>
#include <string_view>

#include <catch.hpp>

TEST_CASE("string_formatters", "") {
    using namespace std::literals;
    using lrstd::format;
    {
        REQUIRE(format("{}", "hey") == "hey");
        REQUIRE(format("{0: ^6}", "hey") == " hey  ");
        REQUIRE(format("{:_>6}", "hey") == "___hey");
    }
    {
        REQUIRE(format("{}", "hey"s) == "hey");
        REQUIRE(format("{0: ^6}", "hey"s) == " hey  ");
        REQUIRE(format("{:_>6}", "hey"s) == "___hey");
    }
    {
        REQUIRE(format("{}", "hey"sv) == "hey");
        REQUIRE(format("{0: ^6}", "hey"sv) == " hey  ");
        REQUIRE(format("{:_>6}", "hey"sv) == "___hey");
    }
}
