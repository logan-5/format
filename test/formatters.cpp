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
    {
        REQUIRE(format("{:}", true) == "true");
        REQUIRE(format("{0: ^6}", true) == " true ");
        REQUIRE(format("{:_>6}", false) == "_false");
    }
    {
        REQUIRE(format("{:s}", true) == "true");
        REQUIRE(format("{0: ^6s}", true) == " true ");
        REQUIRE(format("{:_>6s}", false) == "_false");
    }
    {
        REQUIRE(format("{:c}", true) == "\x1");
        REQUIRE(format("{0: ^6c}", true) == "  \x1   ");
        REQUIRE(format("{:_>6c}", false) == std::string_view("_____\x0", 6));
    }
    {
        REQUIRE(format("{:d}", true) == "1");
        REQUIRE(format("{0: ^6d}", true) == "  1   ");
        REQUIRE(format("{:_>6d}", false) == "_____0");
    }
}
