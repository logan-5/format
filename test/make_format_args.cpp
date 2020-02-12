#include "format.hpp"

#include <iostream>

#include <catch.hpp>

TEST_CASE("make_format_args", "") {
    lrstd::make_format_args('1', '2', '3', '4', '5');

    std::cout << lrstd::format("{1}ello, {0}orld\n", 'h', 'w');
}
