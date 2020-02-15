#include "format.hpp"

#include <string>
#include <string_view>

#include <catch.hpp>

TEST_CASE("string_formatters", "") {
    using namespace std::literals;
    using lrstd::format;
    {
        CHECK(format("{}", "hey") == "hey");
        CHECK(format("{0: ^6}", "hey") == " hey  ");
        CHECK(format("{:_>6}", "hey") == "___hey");
    }
    {
        CHECK(format("{}", "hey"s) == "hey");
        CHECK(format("{0: ^6}", "hey"s) == " hey  ");
        CHECK(format("{:_>6}", "hey"s) == "___hey");
    }
    {
        CHECK(format("{}", "hey"sv) == "hey");
        CHECK(format("{0: ^6}", "hey"sv) == " hey  ");
        CHECK(format("{:_>6}", "hey"sv) == "___hey");
    }
    {
        CHECK(format("{:}", true) == "true");
        CHECK(format("{0: ^6}", true) == " true ");
        CHECK(format("{:_>6}", false) == "_false");
    }
    {
        CHECK(format("{:s}", true) == "true");
        CHECK(format("{0: ^6s}", true) == " true ");
        CHECK(format("{:_>6s}", false) == "_false");
    }
    {
        CHECK(format("{:c}", true) == "\x1");
        CHECK(format("{0: ^6c}", true) == "  \x1   ");
        CHECK(format("{:_>6c}", false) == std::string_view("_____\x0", 6));
    }
    {
        CHECK(format("{:d}", true) == "1");
        CHECK(format("{0: ^6d}", true) == "  1   ");
        CHECK(format("{:_>6d}", false) == "_____0");
    }
}
