#include "format.hpp"

#include <catch.hpp>

TEST_CASE("make_format_args", "") {
    lrstd::make_format_args('1', '2', '3', '4', '5');
}
