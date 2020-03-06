#ifndef LRSTD_FORMAT_COMMON_HPP
#define LRSTD_FORMAT_COMMON_HPP

#include <cassert>

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

}  // namespace lrstd::detail

#endif
