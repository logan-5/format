#include "format.hpp"

#include <catch.hpp>

#include <string>

TEST_CASE("format_error", "") {
    {
        const char* what_arg = "focus shifted";
        lrstd::format_error error = lrstd::format_error(what_arg);
        CHECK(strcmp(error.what(), what_arg) == 0);
    }
    {
        std::string what_arg = "save the organism";
        lrstd::format_error error = lrstd::format_error(what_arg);
        CHECK(strcmp(error.what(), what_arg.c_str()) == 0);
    }
}
