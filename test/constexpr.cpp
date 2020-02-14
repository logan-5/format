#include <catch.hpp>
#include <iostream>
#include "format.hpp"

#include <array>

template <class Range, class InputIt2>
constexpr bool equal(const Range& range, InputIt2 first2) noexcept {
    auto first1 = std::begin(range);
    auto last1 = std::end(range);
    for (; first1 != last1; ++first1, ++first2) {
        if (!(*first1 == *first2)) {
            return false;
        }
    }
    return true;
}

#if LRSTD_USE_EXTRA_CONSTEXPR

#define PASTE2(X, Y) X##Y
#define PASTE(X, Y) PASTE2(X, Y)

#define CONSTEXPR_TEST(RESULT, FMT, ...)                             \
    [[maybe_unused]] constexpr auto PASTE(f, __LINE__) = []() {      \
        std::array<char, 128> buf{};                                 \
        lrstd::format_to(buf.begin(), FMT, __VA_ARGS__);             \
        [[maybe_unused]] constexpr std::string_view result = RESULT; \
        return ::equal(result, buf.cbegin());                        \
    }();                                                             \
    static_assert(PASTE(f, __LINE__))

#else
#define CONSTEXPR_TEST(...)
#endif

CONSTEXPR_TEST("hello, world\n", "{}ello, {}orld\n", 'h', 'w');

TEST_CASE("constexpr", "") {
    {
        std::array<char, 128> buf{};
        lrstd::format_to(buf.begin(), "{}ello, {}orld\n", 'h', 'w');
        REQUIRE(std::string_view(buf.data()) == "hello, world\n");
    }
}
