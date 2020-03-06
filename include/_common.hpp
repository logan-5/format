#ifndef LRSTD_FORMAT_COMMON_HPP
#define LRSTD_FORMAT_COMMON_HPP

#include <cassert>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#define LRSTD_UNREACHABLE() __builtin_unreachable()
#define LRSTD_ASSERT(...) assert(__VA_ARGS__)
#define LRSTD_ALWAYS_INLINE __attribute__((always_inline))

// #define LRSTD_USE_EXTRA_CONSTEXPR false

#if LRSTD_USE_EXTRA_CONSTEXPR
#define LRSTD_EXTRA_CONSTEXPR constexpr
#else
#define LRSTD_EXTRA_CONSTEXPR
#endif

namespace lrstd::detail {

template <class... F>
struct overloaded : F... {
    using F::operator()...;
};
template <class... F>
overloaded(F...)->overloaded<F...>;

template <typename T>
inline constexpr bool is_signed_integer_v =
      std::is_integral_v<T>&& std::is_signed_v<T>;
template <typename T>
inline constexpr bool is_unsigned_integer_v =
      std::is_integral_v<T> && !std::is_signed_v<T>;

template <class It, class = void>
struct hack_is_contiguous_iterator : std::false_type {};

template <class It>
struct hack_is_contiguous_iterator<
      It,
      std::enable_if_t<
            !std::is_void_v<typename std::iterator_traits<It>::value_type>>>
    : std::bool_constant<std::disjunction_v<
            std::is_pointer<It>,
            std::is_same<It,
                         typename std::vector<typename std::iterator_traits<
                               It>::value_type>::iterator>,
            std::is_same<
                  It,
                  typename std::basic_string<typename std::iterator_traits<
                        It>::value_type>::iterator>,
            std::is_same<
                  It,
                  typename std::basic_string_view<typename std::iterator_traits<
                        It>::value_type>::iterator>>> {};

template <class It>
inline constexpr bool hack_is_contiguous_iterator_v =
      hack_is_contiguous_iterator<It>::value;

template <class T>
constexpr T* to_raw_pointer(T* ptr) noexcept {
    return ptr;
}
template <class It, class = std::enable_if_t<hack_is_contiguous_iterator_v<It>>>
constexpr auto* to_raw_pointer(It it) noexcept(noexcept(&*it)) {
    return &*it;
}

template <class It, class = void>
struct maybe_to_raw_pointer_impl {
    constexpr It operator()(It it) const noexcept { return it; }
};
template <class It>
struct maybe_to_raw_pointer_impl<
      It,
      std::void_t<decltype(to_raw_pointer(std::declval<It>()))>> {
    constexpr auto* operator()(It it) const
          noexcept(noexcept(to_raw_pointer(it))) {
        return to_raw_pointer(it);
    }
};

template <class It>
constexpr auto maybe_to_raw_pointer(It it) noexcept(
      noexcept(maybe_to_raw_pointer_impl<It>{}(it))) {
    return maybe_to_raw_pointer_impl<It>{}(it);
}

template <class It, class T>
constexpr It to_iter(T* ptr, It it) noexcept(
      noexcept(std::next(it, std::distance(to_raw_pointer(it), ptr)))) {
    return std::next(it, std::distance(to_raw_pointer(it), ptr));
}
template <class It, class = std::enable_if_t<!std::is_pointer_v<It>>>
constexpr It to_iter(It ptr, It) noexcept {
    return ptr;
}

}  // namespace detail

#endif
