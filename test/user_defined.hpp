#pragma once

#include "converter.hpp"
#include "format.hpp"

enum color { red, green, blue };
inline const char* color_names[] = {"red", "green", "blue"};

template <class Char>
struct lrstd::formatter<color, Char> : lrstd::formatter<const Char*, Char> {
    // TODO: the implementation of formatted_size and format_to_n use special
    // iterators, which requires this to be templated on the output iterator. is
    // that conforming?
    template <class Out>
    auto format(color c, basic_format_context<Out, Char>& ctx) {
        return formatter<const Char*, Char>::format(
              str_fn<Char>{}(color_names[c]), ctx);
    }
};

struct S {
    int value;
};

template <class Char>
struct lrstd::formatter<S, Char> {
    size_t width_arg_id = 0;

    // Parses a width argument id in the format { digit }.
    constexpr auto parse(basic_format_parse_context<Char>& ctx) {
        auto iter = ctx.begin();
        auto get_char = [&]() -> int { return iter != ctx.end() ? *iter : 0; };
        if (get_char() != '{')
            return iter;
        ++iter;
        int c = get_char();
        if (!isdigit(c) || (++iter, get_char()) != '}')
            throw format_error("invalid format");
        width_arg_id = static_cast<std::size_t>(c - '0');
        ctx.check_arg_id(width_arg_id);
        return ++iter;
    }

    template <class Int>
    static constexpr bool in_width_range(Int i) noexcept {
        if constexpr (std::is_signed_v<Int>) {
            if (i < 0)
                return false;
        }
        using Biggest = std::conditional_t<std::is_signed_v<Int>, long long,
                                           unsigned long long>;
        return i <= static_cast<Biggest>(std::numeric_limits<int>::max());
    }

    // Formats an S with width given by the argument width_­arg_­id.
    template <class Out>
    auto format(S s, basic_format_context<Out, Char>& ctx) {
        int width = visit_format_arg(
              [](auto value) -> int {
                  if constexpr (!std::is_integral_v<decltype(value)>)
                      throw format_error("width is not integral");
                  else if (!in_width_range(value))
                      throw format_error("invalid width");
                  else
                      return static_cast<int>(value);
              },
              ctx.arg(width_arg_id));
        return format_to(ctx.out(), "{0:x>{1}}", s.value, width);
    }
};
