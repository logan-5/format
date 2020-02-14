#include "format.hpp"

#include <catch.hpp>

#include <array>
#include <string>
#include <string_view>
#include <vector>

struct SV {
    template <std::size_t N>
    std::string_view operator()(const std::array<char, N>& arr) const {
        return std::string_view{arr.data()};
    }
    template <class C>
    std::string_view operator()(const C& c) const {
        return std::string_view{std::data(c), std::size(c)};
    }
};

TEST_CASE("output", "") {
    using namespace std::string_view_literals;
    using namespace lrstd;

    SV sv;

    {
        std::array<char, 128> chars{};
        format_to(chars.begin(), "unavoidable paradigm");
        REQUIRE(sv(chars) == "unavoidable paradigm");
        format_to(chars.begin(), "the system is {1} free from {0}", "error",
                  "now");
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
