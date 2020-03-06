#ifndef LRSTD_FORMAT_WRITER_HPP
#define LRSTD_FORMAT_WRITER_HPP

#include "_common.hpp"

#include <algorithm>
#include <string_view>

namespace lrstd::detail {

struct single_char_writer {
    template <class CharT, class Out>
    constexpr Out operator()(CharT c, Out out) const
          noexcept(noexcept(*out++ = c)) {
        *out++ = c;
        return out;
    }
    template <class CharT, class Traits = std::char_traits<CharT>>
    constexpr CharT* operator()(CharT c, CharT* out) const
          noexcept(noexcept(Traits::assign(*out++, c))) {
        Traits::assign(*out++, c);
        return out;
    }
};
struct repeated_char_writer {
    template <class CharT, class Out>
    constexpr Out operator()(CharT c, std::size_t count, Out out) const {
        return std::fill_n(out, count, c);
    }
#if !LRSTD_USE_EXTRA_CONSTEXPR
    template <class CharT, class Traits = std::char_traits<CharT>>
    constexpr CharT* operator()(CharT c, std::size_t count, CharT* out) const
          noexcept(noexcept(Traits::assign(out, count, c))) {
        Traits::assign(out, count, c);
        return out + count;
    }
#endif

    // build_buf_iter optimization
    template <class CharT, class OutCharT>
    constexpr build_buf_iter<OutCharT>
    operator()(CharT c, std::size_t count, build_buf_iter<OutCharT> out) const {
        return out.write(c, count);
    }

    // write_n optimization
    template <class CharT, class It>
    constexpr write_n_iter<It> operator()(CharT c,
                                          std::size_t count,
                                          write_n_iter<It> w) const
          noexcept(noexcept((*this)(c, count, w.it))) {
        const auto to_write = std::min(count, w.remaining());
        w.it = (*this)(c, to_write, w.it);
        w.current += to_write;
        if (count > to_write) {
            w.overflow += count - to_write;
        }
        return w;
    }
};

struct str_writer_common {
    template <class CharT, class Traits, class Out>
    constexpr Out operator()(std::basic_string_view<CharT, Traits> str,
                             Out out) const {
        return std::copy(str.begin(), str.end(), out);
    }

    // build_buf_iter optimization
    template <class CharT, class Traits, class OutCharT>
    constexpr build_buf_iter<OutCharT> operator()(
          std::basic_string_view<CharT, Traits> str,
          build_buf_iter<OutCharT> out) const {
        return out.write(str);
    }
};
template <class Derived>
struct str_write_n_optimization {
    template <class CharT, class Traits, class It>
    constexpr write_n_iter<It> operator()(
          std::basic_string_view<CharT, Traits> str,
          write_n_iter<It> w) const
          noexcept(noexcept(static_cast<const Derived&> (*this)(str, w.it))) {
        auto truncated_str = str.substr(0, w.remaining());
        w.it = static_cast<const Derived&>(*this)(truncated_str, w.it);
        w.current += truncated_str.size();
        w.overflow += str.size() - truncated_str.size();
        return w;
    }
};
struct overlapping_str_writer
    : str_writer_common
    , str_write_n_optimization<overlapping_str_writer> {
    using str_writer_common::operator();
    using str_write_n_optimization<overlapping_str_writer>::operator();

#if !LRSTD_USE_EXTRA_CONSTEXPR
    template <class CharT, class Traits>
    constexpr CharT* operator()(std::basic_string_view<CharT, Traits> str,
                                CharT* out) const noexcept(false) {
        std::char_traits<CharT>::move(out, str.data(), str.size());
        return out + str.size();
    }
#endif
};
struct overlapping_generic_writer
    : overloaded<overlapping_str_writer,
                 single_char_writer,
                 repeated_char_writer> {};

struct nonoverlapping_str_writer
    : str_writer_common
    , str_write_n_optimization<nonoverlapping_str_writer> {
    using str_writer_common::operator();
    using str_write_n_optimization<nonoverlapping_str_writer>::operator();
#if !LRSTD_USE_EXTRA_CONSTEXPR
    template <class CharT, class Traits>
    constexpr CharT* operator()(std::basic_string_view<CharT, Traits> str,
                                CharT* out) const noexcept(false) {
        std::char_traits<CharT>::copy(out, str.data(), str.size());
        return out + str.size();
    }
#endif
};
struct nonoverlapping_generic_writer
    : overloaded<nonoverlapping_str_writer,
                 single_char_writer,
                 repeated_char_writer> {};

}  // namespace lrstd::detail

#endif
