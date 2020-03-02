#include "format.hpp"
#include "user_defined.hpp"

#include <catch.hpp>

#include <string>
#include <string_view>
#include <type_traits>

template <class T>
constexpr bool formatter_constructible_v =
      (std::is_default_constructible_v<T> && std::is_copy_constructible_v<T> &&
       std::is_copy_assignable_v<T> && std::is_destructible_v<T>);

template <class T>
constexpr bool is_formatter_v = formatter_constructible_v<T>;

static_assert(is_formatter_v<lrstd::formatter<char, char>>);
static_assert(is_formatter_v<lrstd::formatter<char, wchar_t>>);
static_assert(is_formatter_v<lrstd::formatter<wchar_t, wchar_t>>);

TEMPLATE_TEST_CASE("string_formatters", "", char, wchar_t) {
    using charT = TestType;
    static_assert(is_formatter_v<lrstd::formatter<charT*, charT>>);
    static_assert(is_formatter_v<lrstd::formatter<const charT*, charT>>);
    static_assert(is_formatter_v<lrstd::formatter<const charT[5], charT>>);
    static_assert(
          is_formatter_v<lrstd::formatter<std::basic_string<charT>, charT>>);
    static_assert(is_formatter_v<
                  lrstd::formatter<std::basic_string_view<charT>, charT>>);
}

TEMPLATE_TEST_CASE("arithmetic_formatters",
                   "",
                   signed char,
                   unsigned char,
                   short int,
                   unsigned short int,
                   int,
                   unsigned int,
                   long int,
                   unsigned long int,
                   long long int,
                   unsigned long long int,
                   float,
                   double,
                   long double) {
    static_assert(is_formatter_v<lrstd::formatter<TestType, char>>);
    static_assert(is_formatter_v<lrstd::formatter<TestType, wchar_t>>);
}

TEMPLATE_TEST_CASE("pointer_formatters", "", char, wchar_t) {
    using charT = TestType;
    static_assert(is_formatter_v<lrstd::formatter<std::nullptr_t, charT>>);
    static_assert(is_formatter_v<lrstd::formatter<void*, charT>>);
    static_assert(is_formatter_v<lrstd::formatter<const void*, charT>>);
}

static_assert(is_formatter_v<lrstd::formatter<color, char>>);
static_assert(is_formatter_v<lrstd::formatter<color, wchar_t>>);

template <class T>
constexpr bool disabled_constructible_v =
      (!std::is_default_constructible_v<T> &&
       !std::is_copy_constructible_v<T> && !std::is_move_constructible_v<T> &&
       !std::is_copy_assignable_v<T> && !std::is_move_assignable_v<T>);

template <class T>
constexpr bool is_disabled_v = disabled_constructible_v<T>;

static_assert(!is_formatter_v<lrstd::formatter<wchar_t, char>>);
static_assert(is_disabled_v<lrstd::formatter<wchar_t, char>>);

static_assert(!is_formatter_v<lrstd::formatter<const char*, wchar_t>>);
static_assert(is_disabled_v<lrstd::formatter<const char*, wchar_t>>);

struct MyType {};

TEMPLATE_TEST_CASE("disabled_formatters",
                   "",
                   MyType,
                   /*char8_t,*/ char16_t,
                   char32_t) {
    static_assert(!is_formatter_v<lrstd::formatter<TestType, char>>);
    static_assert(!is_formatter_v<lrstd::formatter<TestType, wchar_t>>);
    static_assert(is_disabled_v<lrstd::formatter<TestType, char>>);
    static_assert(is_disabled_v<lrstd::formatter<TestType, wchar_t>>);
}
