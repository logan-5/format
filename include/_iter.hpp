#ifndef LRSTD_FORMAT_ITER_HPP
#define LRSTD_FORMAT_ITER_HPP

#include "_common.hpp"

#include <cstdint>
#include <iterator>
#include <type_traits>

namespace lrstd::detail {

template <class CharT>
inline constexpr bool is_char_or_wchar_t_v =
      std::is_same_v<char, CharT> || std::is_same_v<wchar_t, CharT>;

class count_iter {
    std::size_t _count = 0;

   public:
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    constexpr count_iter& operator*() noexcept { return *this; }
    constexpr count_iter& operator++() noexcept { return *this; }
    constexpr count_iter& operator++(int) noexcept { return *this; }

    template <class CharT,
              class = std::enable_if_t<is_char_or_wchar_t_v<CharT>>>
    constexpr count_iter& operator=(CharT) noexcept {
        ++_count;
        return *this;
    }

    constexpr std::size_t count() const noexcept { return _count; }
};

template <class It>
using iter_difference_t = std::ptrdiff_t;

template <class It>
class write_n_iter {
   public:
    iter_difference_t<It> n;
    It it;
    iter_difference_t<It> current;
    iter_difference_t<It> overflow;

    constexpr std::size_t remaining() const noexcept {
        LRSTD_ASSERT(n >= current);
        return static_cast<std::size_t>(n - current);
    }

   public:
    constexpr write_n_iter(iter_difference_t<It> n, It it)
        : n{n}, it{it}, current{0}, overflow{0} {}

    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

    constexpr write_n_iter& operator*() noexcept { return *this; }
    constexpr write_n_iter& operator++() noexcept { return *this; }
    constexpr write_n_iter& operator++(int) noexcept { return *this; }

    template <class CharT,
              class = std::enable_if_t<is_char_or_wchar_t_v<CharT>>>
    constexpr write_n_iter& operator=(CharT c) noexcept {
        if (current < n) {
            *it++ = c;
            ++current;
        } else
            ++overflow;
        return *this;
    }

    constexpr It iter() const { return it; }
    constexpr iter_difference_t<It> count() const noexcept {
        return current + overflow;
    }
};

}  // namespace lrstd::detail

#endif
