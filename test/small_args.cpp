#include "converter.hpp"

#include "format.hpp"

#include <catch.hpp>

#include <array>
#include <tuple>

#define ARITY_TEST(ARITY)                                                      \
    do {                                                                       \
        std::array<int, (ARITY)> array{};                                      \
        std::basic_string<TestType> fmt;                                       \
        for (int i = 0; i != (ARITY); ++i)                                     \
            fmt += str("{}");                                                  \
        CHECK(std::apply(                                                      \
                    [&](const auto&... args) { return format(fmt, args...); }, \
                    array) == std::basic_string<TestType>((ARITY), str('0'))); \
        CHECK(std::apply(                                                      \
                    [&](const auto&... args) {                                 \
                        return format_args(make_format_args(args...));         \
                    },                                                         \
                    array)                                                     \
                    ._get_size() == ARITY);                                    \
    } while (false)

TEMPLATE_TEST_CASE("small_args", "", char, wchar_t) {
    using namespace lrstd;
    str_fn<TestType> str;

    ARITY_TEST(0);
    ARITY_TEST(detail::small_arg_count / 2);
    ARITY_TEST(detail::small_arg_count - 1);
    ARITY_TEST(detail::small_arg_count);
    ARITY_TEST(detail::small_arg_count + 1);
    ARITY_TEST(detail::small_arg_count * 2);

    ARITY_TEST(1);
    ARITY_TEST(8);
    ARITY_TEST(16);
    ARITY_TEST(20);
    ARITY_TEST(32);
    ARITY_TEST(64);
    ARITY_TEST(128);
    ARITY_TEST(256);
    ARITY_TEST(512);
    ARITY_TEST(1024);
}
