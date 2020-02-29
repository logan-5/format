#ifndef LRSTD_FORMAT_HPP
#define LRSTD_FORMAT_HPP

#include <array>
#include <cassert>
#include <charconv>
#include <cstring>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>

namespace lrstd {

#define LRSTD_UNREACHABLE() __builtin_unreachable()
#define LRSTD_ASSERT(...) assert(__VA_ARGS__)

// #define LRSTD_USE_EXTRA_CONSTEXPR false

#if LRSTD_USE_EXTRA_CONSTEXPR
#define LRSTD_EXTRA_CONSTEXPR constexpr
#else
#define LRSTD_EXTRA_CONSTEXPR
#endif

// clang-format off
/*
namespace std {
  // [format.error], class format_error
  class format_error;

  // [format.formatter], formatter
  template<class charT> class basic_format_parse_context;
  using format_parse_context = basic_format_parse_context<char>;
  using wformat_parse_context = basic_format_parse_context<wchar_t>;
  
  template<class Out, class charT> class basic_format_context;
  using format_context = basic_format_context<unspecified, char>;
  using wformat_context = basic_format_context<unspecified, wchar_t>;

  template<class T, class charT = char> struct formatter;
  
  // [format.arguments], arguments
  template<class Context> class basic_format_arg;

  template<class Visitor, class Context>
    see below visit_format_arg(Visitor&& vis, basic_format_arg<Context> arg);

  template<class Context, class... Args> struct format-arg-store; // exposition only

  template<class Context> class basic_format_args;
  using format_args = basic_format_args<format_context>;
  using wformat_args = basic_format_args<wformat_context>;

  template<class Out, class charT>
    using format_args_t = basic_format_args<basic_format_context<Out, charT>>;

  template<class Context = format_context, class... Args>
    format-arg-store<Context, Args...>
      make_format_args(const Args&... args);
  template<class... Args>
    format-arg-store<wformat_context, Args...>
      make_wformat_args(const Args&... args);

  // [format.functions], formatting functions
  template<class... Args>
    string format(string_view fmt, const Args&... args);
  template<class... Args>
    wstring format(wstring_view fmt, const Args&... args);

  string vformat(string_view fmt, format_args args);
  wstring vformat(wstring_view fmt, wformat_args args);

  template<class Out, class... Args>
    Out format_to(Out out, string_view fmt, const Args&... args);
  template<class Out, class... Args>
    Out format_to(Out out, wstring_view fmt, const Args&... args);

  template<class Out>
    Out vformat_to(Out out, string_view fmt, format_args_t<Out, char> args);
  template<class Out>
    Out vformat_to(Out out, wstring_view fmt, format_args_t<Out, wchar_t> args);

  template<class Out>
    struct format_to_n_result {
      Out out;
      iter_difference_t<Out> size;
    };
  
  template<class Out, class... Args>
    format_to_n_result<Out> format_to_n(Out out, iter_difference_t<Out> n,
                                        string_view fmt, const Args&... args);
  template<class Out, class... Args>
    format_to_n_result<Out> format_to_n(Out out, iter_difference_t<Out> n,
                                        wstring_view fmt, const Args&... args);

  template<class... Args>
    size_t formatted_size(string_view fmt, const Args&... args);
  template<class... Args>
    size_t formatted_size(wstring_view fmt, const Args&... args);
}
*/
// clang-format on

class format_error : public std::runtime_error {
   public:
    explicit format_error(const std::string& w) : std::runtime_error{w} {}
    explicit format_error(const char* w) : std::runtime_error{w} {}
};

template <class It>
struct iter_difference {
    using type = typename std::iterator_traits<
          std::remove_reference_t<std::remove_cv_t<It>>>::difference_type;
};
template <class T>
struct iter_difference<std::back_insert_iterator<T>> {
    using type = std::ptrdiff_t;
};
template <class It>
using iter_difference_t = typename iter_difference<It>::type;

template <typename CharT>
using basic_string_view = std::basic_string_view<CharT>;

template <class Out, class CharT>
class basic_format_context;
using format_context =
      basic_format_context<std::back_insert_iterator<std::string>, char>;
using wformat_context =
      basic_format_context<std::back_insert_iterator<std::wstring>, wchar_t>;

template <class CharT>
class basic_format_parse_context;
using format_parse_context = basic_format_parse_context<char>;
using wformat_parse_context = basic_format_parse_context<wchar_t>;

template <class Context>
class basic_format_args;
using format_args = basic_format_args<format_context>;
using wformat_args = basic_format_args<wformat_context>;
template <class Out, class CharT>
using format_args_t = basic_format_args<basic_format_context<Out, CharT>>;

template <class T, class CharT>
struct formatter;

namespace detail {
template <class CharT, class Out>
LRSTD_EXTRA_CONSTEXPR Out vformat_to_impl(Out out,
                                          basic_string_view<CharT> fmt,
                                          format_args_t<Out, CharT>& args);
}  // namespace detail

template <class CharT>
class basic_format_parse_context {
   public:
    using char_type = CharT;
    using const_iterator = typename basic_string_view<CharT>::const_iterator;
    using iterator = const_iterator;

   private:
    iterator _begin;
    iterator _end;
    enum class indexing : char { unknown, manual, automatic };
    indexing _indexing;
    std::size_t _next_arg_id;
    std::size_t _num_args;

    void ncc() {}

    template <class C, class O>
    friend LRSTD_EXTRA_CONSTEXPR O
    detail::vformat_to_impl(O out,
                            basic_string_view<C> fmt,
                            format_args_t<O, C>& args);

   public:
    explicit constexpr basic_format_parse_context(basic_string_view<CharT> fmt,
                                                  size_t num_args = 0) noexcept
        : _begin{fmt.begin()}
        , _end{fmt.end()}
        , _indexing{indexing::unknown}
        , _next_arg_id{0}
        , _num_args{num_args} {}

    basic_format_parse_context(const basic_format_parse_context&) = delete;
    basic_format_parse_context& operator=(const basic_format_parse_context&) =
          delete;

    constexpr const_iterator begin() const noexcept { return _begin; }
    constexpr const_iterator end() const noexcept { return _end; }
    constexpr void advance_to(const_iterator it) noexcept { _begin = it; }

    constexpr std::size_t next_arg_id() {
        if (_indexing != indexing::manual) {
            _indexing = indexing::automatic;
            return _next_arg_id++;
        }
        throw format_error{"mixing of automatic and manual argument indexing"};
    }
    constexpr void check_arg_id(std::size_t id_) {
        if (id_ >= _num_args)
            ncc();
        if (_indexing != indexing::automatic)
            _indexing = indexing::manual;
        else
            throw format_error{
                  "mixing of automatic and manual argument indexing"};
    }
};

namespace detail {
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

struct char_counter {
    std::size_t count = 0;
};
template <class It>
struct write_n_wrapper {
    iter_difference_t<It> n;
    It it;
    iter_difference_t<It> current = 0;
    iter_difference_t<It> overflow = 0;
};

}  // namespace detail

namespace detail {
template <class Context, class... Args>
struct args_storage;

enum class arg_tag : char {
    empty,
    b,
    c,
    i,
    ui,
    lli,
    ulli,
    d,
    ld,
    cptr,
    sv,
    vptr,
    h,
};
}  // namespace detail

template <class Context>
struct basic_format_arg {
   public:
    class handle {
        using char_type = typename Context::char_type;
        const void* _ptr;
        void (*_format)(basic_format_parse_context<char_type>&,
                        Context&,
                        const void*);

        template <class T>
        explicit LRSTD_EXTRA_CONSTEXPR handle(const T& val) noexcept
            : _ptr{std::addressof(val)}
            , _format{[](basic_format_parse_context<char_type>& pc,
                         Context& fc,
                         const void* ptr) {
                typename Context::template formatter_type<T> f;
                pc.advance_to(f.parse(pc));
                fc.advance_to(f.format(*static_cast<const T*>(ptr), fc));
            }} {}

        friend struct basic_format_arg;

       public:
        LRSTD_EXTRA_CONSTEXPR void format(
              basic_format_parse_context<char_type>& pc,
              Context& fc) const {
            _format(pc, fc, _ptr);
        }
    };

   private:
    using char_type = typename Context::char_type;

    union arg_storage {
        std::monostate empty;
        bool b;
        char_type c;
        int i;
        unsigned int ui;
        long long int lli;
        unsigned long long int ulli;
        double d;
        long double ld;
        const char_type* cptr;
        basic_string_view<char_type> sv;
        const void* vptr;
        handle h;

        constexpr arg_storage(std::monostate e) noexcept : empty{e} {}
        constexpr arg_storage(bool b) noexcept : b{b} {}
        constexpr arg_storage(char_type c) noexcept : c{c} {}
        constexpr arg_storage(int i) noexcept : i{i} {}
        constexpr arg_storage(unsigned int ui) noexcept : ui{ui} {}
        constexpr arg_storage(long long int lli) noexcept : lli{lli} {}
        constexpr arg_storage(unsigned long long int ulli) noexcept
            : ulli{ulli} {}
        constexpr arg_storage(double d) noexcept : d{d} {}
        constexpr arg_storage(long double ld) noexcept : ld{ld} {}
        constexpr arg_storage(const char_type* cptr) noexcept : cptr{cptr} {}
        constexpr arg_storage(basic_string_view<char_type> sv) noexcept
            : sv{sv} {}
        constexpr arg_storage(const void* vptr) noexcept : vptr{vptr} {}
        constexpr arg_storage(handle h) noexcept : h{h} {}
    };
    struct storage_and_tag {
        using tag_t = detail::arg_tag;
        arg_storage storage;
        tag_t tag;
        constexpr storage_and_tag(std::monostate e = {}) noexcept
            : storage{e}, tag{tag_t::empty} {}
        constexpr storage_and_tag(bool b) noexcept
            : storage{b}, tag{tag_t::b} {}
        constexpr storage_and_tag(char_type c) noexcept
            : storage{c}, tag{tag_t::c} {}
        constexpr storage_and_tag(int i) noexcept : storage{i}, tag{tag_t::i} {}
        constexpr storage_and_tag(unsigned int ui) noexcept
            : storage{ui}, tag{tag_t::ui} {}
        constexpr storage_and_tag(long long int lli) noexcept
            : storage{lli}, tag{tag_t::lli} {}
        constexpr storage_and_tag(unsigned long long int ulli) noexcept
            : storage{ulli}, tag{tag_t::ulli} {}
        constexpr storage_and_tag(double d) noexcept
            : storage{d}, tag{tag_t::d} {}
        constexpr storage_and_tag(long double ld) noexcept
            : storage{ld}, tag{tag_t::ld} {}
        constexpr storage_and_tag(const char_type* cptr) noexcept
            : storage{cptr}, tag{tag_t::cptr} {}
        constexpr storage_and_tag(basic_string_view<char_type> sv) noexcept
            : storage{sv}, tag{tag_t::sv} {}
        constexpr storage_and_tag(const void* vptr) noexcept
            : storage{vptr}, tag{tag_t::vptr} {}
        constexpr storage_and_tag(handle h) noexcept
            : storage{h}, tag{tag_t::h} {}
    };
    storage_and_tag value;

    template <class T,
              typename = std::enable_if_t<std::is_same_v<
                    decltype(typename Context::template formatter_type<T>()
                                   .format(std::declval<const T&>(),
                                           std::declval<Context&>())),
                    typename Context::iterator>>>
    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(const T& v) noexcept
        : value{[&] {
            if constexpr (std::is_same_v<T, bool> ||
                          std::is_same_v<T, char_type>) {
                return v;
            } else if constexpr (std::is_same_v<T, char> &&
                                 std::is_same_v<char_type, wchar_t>) {
                return static_cast<wchar_t>(v);
            } else if constexpr (detail::is_signed_integer_v<T> &&
                                 sizeof(T) <= sizeof(int)) {
                return static_cast<int>(v);
            } else if constexpr (detail::is_unsigned_integer_v<T> &&
                                 sizeof(T) <= sizeof(unsigned int)) {
                return static_cast<unsigned int>(v);
            } else if constexpr (detail::is_signed_integer_v<T> &&
                                 sizeof(T) <= sizeof(long long int)) {
                return static_cast<long long int>(v);
            } else if constexpr (detail::is_unsigned_integer_v<T> &&
                                 sizeof(T) <= sizeof(unsigned long long int)) {
                return static_cast<unsigned long long int>(v);
            } else {
                return handle(v);
            }
        }()} {}

    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(float n) noexcept
        : value{static_cast<double>(n)} {}
    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(double n) noexcept
        : value{n} {}
    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(long double n) noexcept
        : value{n} {}
    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(const char_type* s) noexcept
        : value{s} {}

    template <class Traits>
    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(
          std::basic_string_view<char_type, Traits> s) noexcept
        : value{s} {}
    template <class Traits, class Alloc>
    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(
          const std::basic_string<char_type, Traits, Alloc>& s) noexcept
        : value{s} {}

    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(std::nullptr_t) noexcept
        : value{static_cast<const void*>(nullptr)} {}

    template <class T, class = std::enable_if_t<std::is_void_v<T>>>
    LRSTD_EXTRA_CONSTEXPR explicit basic_format_arg(T* p) noexcept : value{p} {}

    template <class Ctx, class... Args>
    friend LRSTD_EXTRA_CONSTEXPR detail::args_storage<Ctx, Args...>
    make_format_args(const Args&...);

    template <class Visitor, class Ctx>
    friend LRSTD_EXTRA_CONSTEXPR auto visit_format_arg(Visitor&&,
                                                       basic_format_arg<Ctx>);

   public:
    LRSTD_EXTRA_CONSTEXPR basic_format_arg() noexcept = default;

    LRSTD_EXTRA_CONSTEXPR explicit operator bool() const noexcept {
        return value.tag != detail::arg_tag::empty;
    }
};  // namespace lrstd

template <class Visitor, class Context>
LRSTD_EXTRA_CONSTEXPR auto visit_format_arg(Visitor&& visitor,
                                            basic_format_arg<Context> arg) {
    auto visit = [&](auto& arg) {
#if LRSTD_USE_EXTRA_CONSTEXPR
        // invoke() isn't constexpr until C++20 :/
        return std::forward<Visitor>(visitor)(arg);
#else
        return std::invoke(std::forward<Visitor>(visitor), arg);
#endif
    };
    using T = detail::arg_tag;
    switch (arg.value.tag) {
        case T::b:
            return visit(arg.value.storage.b);
        case T::c:
            return visit(arg.value.storage.c);
        case T::i:
            return visit(arg.value.storage.i);
        case T::ui:
            return visit(arg.value.storage.ui);
        case T::lli:
            return visit(arg.value.storage.lli);
        case T::ulli:
            return visit(arg.value.storage.ulli);
        case T::d:
            return visit(arg.value.storage.d);
        case T::ld:
            return visit(arg.value.storage.ld);
        case T::cptr:
            return visit(arg.value.storage.cptr);
        case T::sv:
            return visit(arg.value.storage.sv);
        case T::vptr:
            return visit(arg.value.storage.vptr);
        case T::h:
            return visit(arg.value.storage.h);
        case T::empty:
            return visit(arg.value.storage.empty);
    }
    LRSTD_UNREACHABLE();
}
namespace detail {

template <class Context, class... Args>
struct args_storage {
    std::array<basic_format_arg<Context>, sizeof...(Args)> args;
};
}  // namespace detail

template <class Context>
class basic_format_args {
    std::size_t _size;
    const basic_format_arg<Context>* _data;

    template <class, class>
    friend class basic_format_context;

   public:
    LRSTD_EXTRA_CONSTEXPR basic_format_args() noexcept
        : _size{0}, _data{nullptr} {}

    template <class... Args>
    LRSTD_EXTRA_CONSTEXPR basic_format_args(
          const detail::args_storage<Context, Args...>& storage) noexcept
        : _size{storage.args.size()}, _data{storage.args.data()} {}

    LRSTD_EXTRA_CONSTEXPR basic_format_arg<Context> get(std::size_t i) const
          noexcept {
        return i < _size ? _data[i] : basic_format_arg<Context>();
    }
};

template <class Out, class CharT>
class basic_format_context {
    basic_format_args<basic_format_context> _args;
    Out _out;

    LRSTD_EXTRA_CONSTEXPR basic_format_context(
          const basic_format_args<basic_format_context>& args,
          Out out)
        : _args{args}, _out{out} {}

    template <class C, class O>
    friend LRSTD_EXTRA_CONSTEXPR O
    detail::vformat_to_impl(O out,
                            basic_string_view<C> fmt,
                            format_args_t<O, C>& args);

    constexpr std::size_t args_size() const { return _args._size; }

   public:
    using iterator = Out;
    using char_type = CharT;
    template <class T>
    using formatter_type = formatter<T, CharT>;

    LRSTD_EXTRA_CONSTEXPR basic_format_arg<basic_format_context> arg(
          std::size_t id_) const {
        return _args.get(id_);
    }

    LRSTD_EXTRA_CONSTEXPR iterator out() noexcept { return _out; }
    LRSTD_EXTRA_CONSTEXPR void advance_to(iterator it) { _out = it; }
};

template <class Context = format_context, class... Args>
LRSTD_EXTRA_CONSTEXPR detail::args_storage<Context, Args...> make_format_args(
      const Args&... args) {
    return {basic_format_arg<Context>(args)...};
}

template <class... Args>
LRSTD_EXTRA_CONSTEXPR detail::args_storage<wformat_context, Args...>
make_wformat_args(const Args&... args) {
    return make_format_args<wformat_context>(args...);
}

namespace detail {

template <typename... F>
struct overloaded : public F... {
    using F::operator()...;
};
template <typename... F>
overloaded(F...)->overloaded<F...>;

template <class CharT>
struct range {
    using char_type = CharT;
    using iterator = const CharT*;
    using const_iterator = const CharT*;
    using size_type = std::size_t;
    using traits_type = std::char_traits<CharT>;

    constexpr range() noexcept : _begin{nullptr}, _end{nullptr} {}
    constexpr range(const CharT* begin, const CharT* end) noexcept
        : _begin{begin}, _end{end} {}
    constexpr range(basic_string_view<CharT> sv) noexcept
        : _begin{sv.data()}, _end{sv.data() + sv.size()} {}

    constexpr bool empty() const noexcept { return _begin == _end; }
    constexpr size_type size() const noexcept { return _end - _begin; }
    constexpr iterator begin() const noexcept { return _begin; }
    constexpr iterator end() const noexcept { return _end; }
    constexpr auto rbegin() const noexcept {
        return std::make_reverse_iterator(end());
    }
    constexpr auto rend() const noexcept {
        return std::make_reverse_iterator(begin());
    }

    constexpr void remove_prefix(size_type n) noexcept { _begin += n; }
    constexpr CharT operator[](size_type n) const noexcept { return _begin[n]; }

    constexpr range substr(size_type start) const noexcept {
        return range{_begin + start, _end};
    }
    constexpr range substr(size_type start, size_type count) const noexcept {
        return range{_begin + start, _begin + count};
    }

    constexpr void advance_to(iterator it) noexcept { _begin = it; }

#if LRSTD_USE_EXTRA_CONSTEXPR
    constexpr iterator find(CharT c) const noexcept {
        for (iterator it = _begin; it != _end; ++it) {
            if (*it == c)
                return it;
        }
        return _end;
    }
#else
    iterator find(CharT c) const noexcept {
        auto result = traits_type::find(begin(), size(), c);
        return result ? result : end();
    }
#endif

    constexpr basic_string_view<CharT> as_string_view() const noexcept {
        return basic_string_view<CharT>(_begin, _end - _begin);
    }
    explicit operator basic_string_view<CharT>() const noexcept {
        return as_string_view();
    }

    constexpr bool match_nonempty(CharT c) const noexcept {
        return *begin() == c;
    }
    constexpr bool match(CharT c) const noexcept {
        return !empty() && match_nonempty(c);
    }
    constexpr bool consume(range<CharT> prefix) {
        if (size() >= prefix.size() &&
            traits_type::compare(begin(), prefix.begin(), prefix.size()) == 0) {
            remove_prefix(prefix.size());
            return true;
        }
        return false;
    }
    constexpr bool consume(CharT c) {
        if (match(c)) {
            remove_prefix(1);
            return true;
        }
        return false;
    }

   private:
    const CharT* _begin;
    const CharT* _end;
};

struct parse_integer_result {
    union {
        std::monostate _;
        std::size_t integer;
    };
    enum class type : char { none, error, success } tag;

    constexpr parse_integer_result(type t) : _{}, tag{t} {}
    constexpr parse_integer_result(std::size_t i)
        : integer{i}, tag{type::success} {}

    explicit constexpr operator bool() const noexcept {
        return tag == type::success;
    }
};

#if LRSTD_USE_EXTRA_CONSTEXPR
inline constexpr parse_integer_result parse_integer(range<char>& fmt) {
    range<char>::iterator it = fmt.begin();
    auto is_digit = [](char c) { return '0' <= c && c <= '9'; };
    for (; it != fmt.end() && is_digit(*it); ++it)
        ;
    if (it == fmt.begin())
        return parse_integer_result::type::none;

    std::size_t place_value = 1;
    std::size_t ret = 0;
    for (auto dig_it = std::make_reverse_iterator(it); dig_it != fmt.rend();
         ++dig_it) {
        ret += (*dig_it - '0') * place_value;
        place_value *= 10;
    }
    fmt.advance_to(it);
    return ret;
}
#else
inline parse_integer_result parse_integer(range<char>& fmt) {
    std::size_t val;
    const auto result = std::from_chars(fmt.begin(), fmt.end(), val, 10);
    if (result.ptr == fmt.begin())
        return parse_integer_result::type::none;
    if (result.ec != std::errc())
        return parse_integer_result::type::error;
    fmt.advance_to(result.ptr);
    return val;
}
#endif

inline parse_integer_result parse_integer(range<wchar_t>& fmt) {
    range<wchar_t>::iterator it = fmt.begin();
    auto is_digit = [](wchar_t c) { return L'0' <= c && c <= L'9'; };
    for (; it != fmt.end() && is_digit(*it); ++it)
        ;
    if (it == fmt.begin())
        return parse_integer_result::type::none;

    std::size_t place_value = 1;
    std::size_t ret = 0;
    for (auto dig_it = std::make_reverse_iterator(it); dig_it != fmt.rend();
         ++dig_it) {
        ret += (*dig_it - L'0') * place_value;
        place_value *= 10;
    }
    fmt.advance_to(it);
    return ret;
}

template <class CharT>
constexpr typename range<CharT>::iterator find_balanced_delimiter_end(
      range<CharT> fmt,
      typename range<CharT>::char_type start,
      typename range<CharT>::char_type end,
      std::size_t start_count = 1) {
    auto it = fmt.begin();
    for (; it != fmt.end(); ++it) {
        if (*it == start)
            ++start_count;
        else if (*it == end && --start_count == 0)
            break;
    }
    return it;
}

struct arg_id_t {
    constexpr arg_id_t() noexcept = default;
    constexpr explicit arg_id_t(std::size_t val) noexcept : _id{val} {}

    std::size_t _id{static_cast<std::size_t>(-1)};

    static constexpr arg_id_t auto_id() noexcept {
        return arg_id_t{static_cast<std::size_t>(-1)};
    }
    constexpr bool is_auto() const noexcept {
        return _id == static_cast<std::size_t>(-1);
    }
};

struct integer_or_arg_id {
    union {
        std::size_t integer;
        arg_id_t arg_id;
    };
    enum class which : char { integer, arg_id } tag;
    constexpr integer_or_arg_id(std::size_t i = 0)
        : integer{i}, tag{which::integer} {}
    constexpr integer_or_arg_id(arg_id_t arg)
        : arg_id{arg}, tag{which::arg_id} {}
};

enum class alignment_t : char {
    defaulted,
    left,
    right,
    center,
};
inline constexpr bool is_defaulted(alignment_t a) noexcept {
    return a == alignment_t::defaulted;
}

enum class sign_t : char { minus, plus, space };
struct width_t {
    integer_or_arg_id i{};
    bool zero_pad = false;
};
enum class type_t : char {
    defaulted = ' ',
    a = 'a',
    A = 'A',
    b = 'b',
    B = 'B',
    c = 'c',
    d = 'd',
    e = 'e',
    E = 'E',
    f = 'f',
    F = 'F',
    g = 'g',
    G = 'G',
    n = 'n',
    o = 'o',
    p = 'p',
    s = 's',
    x = 'x',
    X = 'X',
};
inline constexpr bool is_defaulted(type_t t) noexcept {
    return t == type_t::defaulted;
}

struct std_format_spec_base {
    alignment_t align{alignment_t::defaulted};
    bool alternate = false;
    sign_t sign{sign_t::minus};
    width_t width;
    integer_or_arg_id precision;
    type_t type{type_t::defaulted};
};
template <class CharT>
struct std_format_spec : std_format_spec_base {
    CharT fill{' '};
};

template <class CharT>
struct type_chars;
template <>
struct type_chars<char> {
    static constexpr const std::string_view value = "aAbBcdeEfFgGnopsxX";
};
template <>
struct type_chars<wchar_t> {
    static constexpr const std::wstring_view value = L"aAbBcdeEfFgGnopsxX";
};

template <class CharT>
struct std_spec_parser {
    constexpr std_spec_parser(basic_format_parse_context<CharT>& parse_context,
                              std_format_spec<CharT>& spec) noexcept
        : parse_context{parse_context}
        , spec{spec}
        , fmt{make_range(parse_context)} {}

    basic_format_parse_context<CharT>& parse_context;
    std_format_spec<CharT>& spec;
    range<CharT> fmt;

    constexpr void parse() {
        parse_fill_and_align();
        parse_sign();
        if (fmt.consume('#')) {
            spec.alternate = true;
        }
        const bool width_leading_zero = fmt.consume('0');
        if (const auto width = parse_integer_or_arg_id<false>(fmt)) {
            spec.width = width_t{*width, width_leading_zero};
        }
        parse_type();
        parse_context.advance_to(fmt.begin());
    }

   private:
    constexpr bool parse_align(range<CharT>& s) noexcept {
        if (s.consume('<')) {
            spec.align = alignment_t::left;
            return true;
        }
        if (s.consume('>')) {
            spec.align = alignment_t::right;
            return true;
        }
        if (s.consume('^')) {
            spec.align = alignment_t::center;
            return true;
        }
        return false;
    }

    constexpr void parse_fill_and_align() {
        LRSTD_ASSERT(!fmt.empty());
        if (fmt.match_nonempty('{') || fmt.match_nonempty('}'))
            return;
        if (fmt.size() >= 2) {
            auto next = fmt.substr(1);
            if (parse_align(next)) {
                spec.fill = fmt[0];
                fmt = next;
                return;
            }
        }
        if (parse_align(fmt)) {
            spec.fill = ' ';
        }
    }

    constexpr void parse_sign() {
        if (fmt.consume('+'))
            spec.sign = sign_t::plus;
        else if (fmt.consume('-'))
            spec.sign = sign_t::minus;
        else if (fmt.consume(' '))
            spec.sign = sign_t::space;
    }

    constexpr std::optional<integer_or_arg_id> parse_integer_or_arg_id_impl(
          range<CharT>& s) {
        if (s.match('{')) {
            auto next = s.substr(1);
            if (const parse_integer_result result = parse_integer(next)) {
                if (next.consume('}')) {
                    s = next;
                    return arg_id_t{result.integer};
                }
            } else if (next.consume('}')) {
                s = next;
                return arg_id_t{parse_context.next_arg_id()};
            }
        }
        if (const parse_integer_result result = parse_integer(s)) {
            return result.integer;
        }
        return std::nullopt;
    }

    template <bool AllowLeadingZero>
    constexpr std::optional<integer_or_arg_id> parse_integer_or_arg_id(
          range<CharT>& s) {
        if constexpr (!AllowLeadingZero) {
            if (s.match('0'))
                return std::nullopt;
        }
        return parse_integer_or_arg_id_impl(s);
    }

    constexpr void parse_precision() {
        if (fmt.match('.')) {
            auto next = fmt.substr(1);
            if (auto result = parse_integer_or_arg_id<true>(next)) {
                spec.precision = *result;
                fmt = next;
            }
        }
    }

    constexpr void parse_type() {
        for (CharT c : type_chars<CharT>::value) {
            if (fmt.consume(c)) {
                spec.type = type_t(c);
            }
        }
    }

    static constexpr range<CharT> make_range(
          const basic_format_parse_context<CharT>& parse_context) noexcept {
        range<CharT> fmt(parse_context.begin(), parse_context.end());
        LRSTD_ASSERT(!fmt.empty());
        return fmt;
    }
};

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
    };
    template <class CharT>
    constexpr char_counter operator()(CharT, char_counter counter) const
          noexcept {
        ++counter.count;
        return counter;
    }
    template <class CharT, class It>
    constexpr write_n_wrapper<It> operator()(CharT c,
                                             write_n_wrapper<It> w) const
          noexcept(noexcept(this->operator()(c, w.it))) {
        if (w.current < w.n) {
            w.it = this->operator()(c, w.it);
            ++w.current;
            return w;
        }
        ++w.overflow;
        return w;
    }
};
struct repeated_char_writer {
    template <class CharT, class Out>
    constexpr Out operator()(CharT c, std::size_t count, Out out) const
          noexcept(noexcept(*out++ = c)) {
        while (count--)
            *out++ = c;
        return out;
    }
#if !LRSTD_USE_EXTRA_CONSTEXPR
    template <class CharT, class Traits = std::char_traits<CharT>>
    constexpr CharT* operator()(CharT c, std::size_t count, CharT* out) const
          noexcept(noexcept(Traits::assign(out, count, c))) {
        Traits::assign(out, count, c);
        return out + count;
    }
#endif
    template <class CharT>
    constexpr char_counter operator()(CharT,
                                      std::size_t count,
                                      char_counter counter) const noexcept {
        counter.count += count;
        return counter;
    }
    template <class CharT, class It>
    constexpr write_n_wrapper<It> operator()(CharT c,
                                             std::size_t count,
                                             write_n_wrapper<It> w) const
          noexcept(noexcept(this->operator()(c, count, w.it))) {
        const auto to_write = std::min<std::size_t>(count, w.n - w.current);
        w.it = this->operator()(c, to_write, w.it);
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
                             Out out) const
          noexcept(noexcept(*out++ = std::declval<CharT&>())) {
        for (CharT c : str)
            *out++ = c;
        return out;
    }
    template <class CharT, class Traits>
    constexpr char_counter operator()(std::basic_string_view<CharT, Traits> str,
                                      char_counter counter) const noexcept {
        counter.count += str.size();
        return counter;
    }
};
struct overlapping_str_writer : str_writer_common {
    using str_writer_common::operator();
#if !LRSTD_USE_EXTRA_CONSTEXPR
    template <class CharT, class Traits>
    constexpr CharT* operator()(std::basic_string_view<CharT, Traits> str,
                                CharT* out) const noexcept(false) {
        std::char_traits<CharT>::move(out, str.data(), str.size());
        return out + str.size();
    }
#endif
    template <class CharT, class Traits, class It>
    constexpr write_n_wrapper<It> operator()(
          std::basic_string_view<CharT, Traits> str,
          write_n_wrapper<It> w) const
          noexcept(noexcept(this->operator()(str, w.it))) {
        auto truncated_str = str.substr(0, w.n - w.current);
        w.it = this->operator()(truncated_str, w.it);
        w.current += truncated_str.size();
        w.overflow += str.size() - truncated_str.size();
        return w;
    }
};
struct overlapping_generic_writer
    : overloaded<overlapping_str_writer,
                 single_char_writer,
                 repeated_char_writer> {};

struct nonoverlapping_str_writer : str_writer_common {
    using str_writer_common::operator();
#if !LRSTD_USE_EXTRA_CONSTEXPR
    template <class CharT, class Traits>
    constexpr CharT* operator()(std::basic_string_view<CharT, Traits> str,
                                CharT* out) const noexcept(false) {
        std::char_traits<CharT>::copy(out, str.data(), str.size());
        return out + str.size();
    }
#endif
    template <class CharT, class Traits, class It>
    constexpr write_n_wrapper<It> operator()(
          std::basic_string_view<CharT, Traits> str,
          write_n_wrapper<It> w) const
          noexcept(noexcept(this->operator()(str, w.it))) {
        auto truncated_str = str.substr(0, w.n - w.current);
        w.it = this->operator()(truncated_str, w.it);
        w.current += truncated_str.size();
        w.overflow += str.size() - truncated_str.size();
        return w;
    }
};
struct nonoverlapping_generic_writer
    : overloaded<nonoverlapping_str_writer,
                 single_char_writer,
                 repeated_char_writer> {};

struct get_width_func {
    std::size_t operator()(...) const noexcept(false) {
        throw format_error("width argument must be an integral type");
    }
    template <class Int, class = std::enable_if_t<std::is_integral_v<Int>>>
    std::size_t operator()(Int i) const noexcept {
        return i;
    }
};

template <class CharT>
struct std_formatter_driver {
    std::optional<std_format_spec<CharT>> spec;

    constexpr typename basic_format_parse_context<CharT>::iterator parse(
          basic_format_parse_context<CharT>& pc) {
        if (pc.begin() != pc.end()) {
            spec.emplace();
            std_spec_parser<CharT>{pc, *spec}.parse();
            if (pc.begin() != pc.end())
                throw format_error("bad standard format spec string");
        }
        return pc.begin();
    }

    template <class Out>
    constexpr std::size_t get_width(
          const basic_format_context<Out, CharT>& fc) const {
        LRSTD_ASSERT(spec.has_value());
        using I = integer_or_arg_id;
        switch (spec->width.i.tag) {
            case I::which::integer:
                return spec->width.i.integer;
            case I::which::arg_id:
                return visit_format_arg(get_width_func{},
                                        fc.arg(spec->width.i.arg_id._id));
        }
    }

    template <class SpecDelegate>
    constexpr void finalize_spec(SpecDelegate&& spec_delegate) {
        LRSTD_ASSERT(spec.has_value());
        spec_delegate.set_defaults(*spec);
        spec_delegate.verify(*spec);
    }

    template <class Out, class FormatEngine>
    constexpr Out format_to_spec(basic_format_context<Out, CharT>& fc,
                                 FormatEngine&& engine) {
        LRSTD_ASSERT(spec.has_value());
        Out out = fc.out();
        const std::size_t width = get_width(fc);
        out = engine.write_left_padding(width, spec->align, spec->fill, out);
        out = engine.write_value(out);
        out = engine.write_right_padding(width, spec->align, spec->fill, out);
        fc.advance_to(out);
        return out;
    }
};

struct simple_padding_engine {
    alignment_t align;

    template <class CharT, class Out>
    Out write_left(std::size_t width,
                   std::size_t value_width,
                   CharT fill,
                   Out out) const {
        if (value_width >= width)
            return out;
        using A = alignment_t;
        switch (align) {
            case A::left:
                return out;
            case A::center:
                return repeated_char_writer{}(fill, (width - value_width) / 2,
                                              out);
            case A::right:
                return repeated_char_writer{}(fill, width - value_width, out);
            case A::defaulted:
                break;
        }
        LRSTD_UNREACHABLE();
    }
    template <class CharT, class Out>
    Out write_right(std::size_t width,
                    std::size_t value_width,
                    CharT fill,
                    Out out) const {
        if (value_width >= width)
            return out;
        using A = alignment_t;
        switch (align) {
            case A::left:
                return repeated_char_writer{}(fill, width - value_width, out);
                ;
            case A::center: {
                const std::size_t count = width - value_width;
                return repeated_char_writer{}(fill, count / 2 + (count & 1),
                                              out);
            }
            case A::right:
                return out;
            case A::defaulted:
                break;
        }
        LRSTD_UNREACHABLE();
    }
};

struct zero_padding_engine {
    template <class Out>
    Out write_left(std::size_t width,
                   std::size_t value_width,
                   std::string_view prefix,
                   char sign,
                   Out out) const {
        nonoverlapping_generic_writer writer;
        const std::size_t sign_width = sign != '\0';
        if (sign_width)
            out = writer(sign, out);
        if (!prefix.empty())
            out = writer(prefix, out);
        const std::size_t prefix_width = sign_width + prefix.size();
        if (prefix_width >= width)
            return out;
        width -= prefix_width;
        if (value_width >= width)
            return out;
        return writer('0', width - value_width, out);
    }
};

template <class T>
inline constexpr bool is_char_or_wchar_v =
      std::is_same_v<T, char> || std::is_same_v<T, wchar_t>;

template <class T, class CharT, bool Enable = is_char_or_wchar_v<CharT>>
struct formatter_impl {
    formatter_impl() = delete;
    formatter_impl(const formatter_impl&) = delete;
    formatter_impl& operator=(const formatter_impl&) = delete;
};

//////////////////////////////
// integer formatters

template <class Int>
struct format_int_storage_type {
    using type = Int;
};
template <>
struct format_int_storage_type<bool> {
    using type = char;
};

template <class CharT>
struct char_default_engine {
    CharT c;
    template <class Out>
    constexpr Out write_value(Out out) const {
        return single_char_writer{}(c, out);
    }
};

struct char_spec_delegate {
    constexpr void set_defaults(std_format_spec_base& spec) const noexcept {
        if (is_defaulted(spec.type)) {
            spec.type = type_t::c;
        }
        if (is_defaulted(spec.align)) {
            spec.align = spec.type == type_t::c ? alignment_t::left
                                                : alignment_t::right;
        }
    }
    constexpr void verify(const std_format_spec_base&) const {
        // TODO
    }
};

template <class CharT>
struct char_spec_engine : char_default_engine<CharT> {
    template <class Out>
    constexpr Out write_left_padding(std::size_t width,
                                     alignment_t align,
                                     CharT fill,
                                     Out out) const {
        return simple_padding_engine{align}.write_left(width, 1, fill, out);
    }
    template <class Out>
    constexpr Out write_right_padding(std::size_t width,
                                      alignment_t align,
                                      CharT fill,
                                      Out out) const {
        return simple_padding_engine{align}.write_right(width, 1, fill, out);
    }
};

struct bool_default_engine {
    bool b;
    template <class Out>
    constexpr Out write_value(Out out) const {
        const std::string_view s = b ? "true" : "false";
        return nonoverlapping_str_writer{}(s, out);
    }
    constexpr std::size_t value_width() const noexcept { return b ? 4 : 5; }
};

struct bool_spec_delegate {
    constexpr void set_defaults(std_format_spec_base& spec) const noexcept {
        if (is_defaulted(spec.type)) {
            spec.type = type_t::s;
        }
        if (is_defaulted(spec.align)) {
            spec.align = spec.type == type_t::s ? alignment_t::left
                                                : alignment_t::right;
        }
    }
    constexpr void verify(const std_format_spec_base&) const {
        // TODO
    }
};

template <class CharT>
struct bool_spec_engine : bool_default_engine {
    template <class Out>
    constexpr Out write_left_padding(std::size_t width,
                                     alignment_t align,
                                     CharT fill,
                                     Out out) const {
        return simple_padding_engine{align}.write_left(width, value_width(),
                                                       fill, out);
    }
    template <class Out>
    constexpr Out write_right_padding(std::size_t width,
                                      alignment_t align,
                                      CharT fill,
                                      Out out) const {
        return simple_padding_engine{align}.write_right(width, value_width(),
                                                        fill, out);
    }
};

template <class Int>
using temp_stack_buffer = std::array<char, sizeof(Int) * 8>;

template <class Int>
using int_template_type = std::conditional_t<std::is_signed_v<Int>,
                                             long long int,
                                             unsigned long long int>;

template <class Int>
struct integer_default_engine {
    Int i;
    template <class Out>
    constexpr Out write_value(Out out) const {
        temp_stack_buffer<Int> buf;
        auto result = std::to_chars(buf.data(), buf.data() + buf.size(), i, 10);
        return nonoverlapping_str_writer{}(
              std::string_view(buf.data(), result.ptr - buf.data()), out);
    }
};

struct integer_spec_engine_common {
    static constexpr char get_sign_char(sign_t sign, bool negative) noexcept {
        using S = sign_t;
        switch (sign) {
            case S::plus:
                return negative ? '-' : '+';
            case S::minus:
                return negative ? '-' : '\0';
            case S::space:
                return negative ? '-' : ' ';
        }
        LRSTD_UNREACHABLE();
    }

    static constexpr basic_string_view<char> get_prefix(
          type_t type,
          bool alternate) noexcept {
        if (!alternate)
            return {};
        using T = type_t;
        switch (type) {
            case T::x:
                return "0x";
            case T::X:
                return "0X";
            case T::b:
                return "0b";
            case T::B:
                return "0B";
            case T::o:
                return "0";

                // clang-format off
					case T::d: case T::c: case T::n: case T::s: case T::a: case T::A:
					case T::e: case T::E: case T::f: case T::F: case T::g: case T::G:
					case T::p:
						return {};
                // clang-format on

            case T::defaulted:
                break;
        }
        LRSTD_UNREACHABLE();
    }
    static constexpr int get_base(type_t type) noexcept {
        using T = type_t;
        switch (type) {
            case T::d:
                return 10;
            case T::x:
            case T::X:
                return 16;
            case T::b:
            case T::B:
                return 2;
            case T::o:
                return 8;

                // clang-format off
					case T::c: case T::n: case T::s: case T::a: case T::A:
					case T::e: case T::E: case T::f: case T::F: case T::g: case T::G:
					case T::p: case T::defaulted:
                // clang-format on
                break;
        }
        LRSTD_UNREACHABLE();
    }
};

template <class Int>
struct integer_spec_engine_base : integer_spec_engine_common {
   protected:
    typename format_int_storage_type<Int>::type i;
    char sign_char;
    std::string_view prefix;

   private:
    temp_stack_buffer<Int> buf;

   protected:
    char* buf_ptr;
    std::size_t buf_size;

   public:
    integer_spec_engine_base(Int i, const std_format_spec_base& spec)
        : i{i}
        , sign_char{get_sign_char(spec.sign, i < 0)}
        , prefix{get_prefix(spec.type, spec.alternate)} {
        auto result = std::to_chars(buf.data(), buf.data() + buf.size(),
                                    this->i, get_base(spec.type));
        buf_ptr = buf.data() + (i < 0);
        buf_size = result.ptr - buf_ptr;
        if (spec.type == type_t::X) {
            std::transform(buf_ptr, buf_ptr + buf_size, buf_ptr,
                           [](char c) { return std::toupper(c); });
        }
    }
};

struct integer_spec_delegate {
    constexpr void set_defaults(std_format_spec_base& spec) noexcept {
        if (is_defaulted(spec.align)) {
            spec.align = alignment_t::right;
        } else {
            spec.width.zero_pad = false;
        }
        if (is_defaulted(spec.type)) {
            spec.type = type_t::d;
        }
    }
    constexpr void verify(const std_format_spec_base&) const {
        // TODO
    }
};

template <class Int, class CharT>
struct integer_simple_spec_engine : integer_spec_engine_base<Int> {
    using base = integer_spec_engine_base<Int>;

    std::size_t value_width() const noexcept { return base::buf_size; }

    template <class Out>
    constexpr Out write_left_padding(std::size_t width,
                                     alignment_t align,
                                     CharT fill,
                                     Out out) const {
        return simple_padding_engine{align}.write_left(
              width,
              value_width() + base::prefix.size() + (base::sign_char != '\0'),
              fill, out);
    }
    template <class Out>
    constexpr Out write_right_padding(std::size_t width,
                                      alignment_t align,
                                      CharT fill,
                                      Out out) const {
        return simple_padding_engine{align}.write_right(
              width,
              value_width() + base::prefix.size() + (base::sign_char != '\0'),
              fill, out);
    }
    template <class Out>
    constexpr Out write_value(Out out) const {
        nonoverlapping_generic_writer writer;
        if (base::sign_char != '\0')
            out = writer(base::sign_char, out);
        if (!base::prefix.empty())
            out = writer(base::prefix, out);
        return writer(std::string_view(base::buf_ptr, base::buf_size), out);
    }
};
template <class Int, class CharT>
struct integer_zero_pad_spec_engine : integer_spec_engine_base<Int> {
    using base = integer_spec_engine_base<Int>;

    std::size_t value_width() const noexcept { return base::buf_size; }

    template <class Out>
    constexpr Out write_left_padding(std::size_t width,
                                     alignment_t,
                                     CharT,
                                     Out out) const {
        return zero_padding_engine{}.write_left(
              width, value_width(), base::prefix, base::sign_char, out);
    }
    template <class Out>
    constexpr Out write_right_padding(std::size_t,
                                      alignment_t,
                                      CharT,
                                      Out out) const {
        return out;
    }
    template <class Out>
    constexpr Out write_value(Out out) const {
        return nonoverlapping_str_writer{}(
              std::string_view(base::buf_ptr, base::buf_size), out);
    }
};

template <class Char, class Int>
constexpr bool representable_as_char(Int i) noexcept {
    if constexpr (sizeof(Int) <= sizeof(Char)) {
        if constexpr (std::is_signed_v<Int> == std::is_signed_v<Char>)
            return true;
        else if constexpr (std::is_signed_v<Int> && !std::is_signed_v<Char>) {
            return 0 <= i;
        } else {
            return i <= static_cast<Int>(std::numeric_limits<Char>::max());
        }
    } else if constexpr (std::is_signed_v<Int>) {
        return static_cast<Int>(std::numeric_limits<Char>::min()) <= i &&
               i <= static_cast<Int>(std::numeric_limits<Char>::max());
    } else {
        return i <= static_cast<Int>(std::numeric_limits<Char>::max());
    }
}

template <class CharT, class SpecDelegate>
struct int_formatter_base : public std_formatter_driver<CharT> {
    using base = std_formatter_driver<CharT>;

   protected:
    template <class Int, typename Out>
    constexpr Out format_impl(Int i, basic_format_context<Out, CharT>& fc) {
        base::finalize_spec(SpecDelegate{});

        using T = type_t;
        switch (base::spec->type) {
            case T::c:
                if (!representable_as_char<CharT>(i)) {
                    throw format_error("value not representable as char");
                }
                return base::format_to_spec(
                      fc, char_spec_engine<CharT>{{static_cast<CharT>(i)}});
            case T::s:
                return base::format_to_spec(
                      fc, bool_spec_engine<CharT>{{static_cast<bool>(i)}});
            default:
                break;
        }
        if (base::spec->width.zero_pad)
            return base::format_to_spec(
                  fc,
                  integer_zero_pad_spec_engine<Int, CharT>{{i, *base::spec}});
        return base::format_to_spec(
              fc, integer_simple_spec_engine<Int, CharT>{{i, *base::spec}});
    }
};

template <class Int, class CharT, class SpecDelegate, class DefaultEngine>
struct int_formatter : public int_formatter_base<CharT, SpecDelegate> {
    using base = int_formatter_base<CharT, SpecDelegate>;
    template <typename Out>
    constexpr Out format(Int i, basic_format_context<Out, CharT>& fc) {
        if (!base::spec) {
            fc.advance_to(DefaultEngine{i}.write_value(fc.out()));
            return fc.out();
        }
        return base::format_impl(static_cast<int_template_type<Int>>(i), fc);
    }
};

template <class CharT>
struct formatter_impl<signed char, CharT, true>
    : public int_formatter<
            signed char,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<signed char>>> {};
template <class CharT>
struct formatter_impl<unsigned char, CharT, true>
    : public int_formatter<
            unsigned char,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<unsigned char>>> {};
template <class CharT>
struct formatter_impl<short int, CharT, true>
    : public int_formatter<
            short int,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<short int>>> {};
template <class CharT>
struct formatter_impl<unsigned short int, CharT, true>
    : public int_formatter<
            unsigned short int,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<unsigned short int>>> {};
template <class CharT>
struct formatter_impl<int, CharT, true>
    : public int_formatter<int,
                           CharT,
                           integer_spec_delegate,
                           integer_default_engine<int_template_type<int>>> {};
template <class CharT>
struct formatter_impl<unsigned int, CharT, true>
    : public int_formatter<
            unsigned int,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<unsigned int>>> {};
template <class CharT>
struct formatter_impl<long int, CharT, true>
    : public int_formatter<
            long int,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<long int>>> {};
template <class CharT>
struct formatter_impl<unsigned long int, CharT, true>
    : public int_formatter<
            unsigned long int,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<unsigned long int>>> {};
template <class CharT>
struct formatter_impl<long long int, CharT, true>
    : public int_formatter<
            long long int,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<long long int>>> {};
template <class CharT>
struct formatter_impl<unsigned long long int, CharT, true>
    : public int_formatter<
            unsigned long long int,
            CharT,
            integer_spec_delegate,
            integer_default_engine<int_template_type<unsigned long long int>>> {
};

template <class CharT>
struct formatter_impl<bool, CharT, true>
    : public int_formatter<bool,
                           CharT,
                           bool_spec_delegate,
                           bool_default_engine> {};

template <class CharT>
struct formatter_impl<CharT, CharT, true>
    : public int_formatter<CharT,
                           CharT,
                           char_spec_delegate,
                           char_default_engine<CharT>> {};
template <>
struct formatter_impl<char, wchar_t, true>
    : public int_formatter<char,
                           wchar_t,
                           char_spec_delegate,
                           char_default_engine<char>> {};

//////////////////////////////
// float formatters

template <class Float, class CharT>
struct float_formatter : public std_formatter_driver<CharT> {
    using base = std_formatter_driver<CharT>;
    template <typename Out>
    constexpr Out format(Float, basic_format_context<Out, CharT>&) {
        throw "not yet implemented";
        // fc.advance_to(
        //       to_iter(base::do_format(maybe_to_raw_pointer(fc.out()),
        //                               base::get_width(fc),
        //                               format_int<Int>{i},
        //                               nonoverlapping_str_writer{}),
        //               fc.out()));
        // return fc.out();
    }
};
template <class CharT>
struct formatter_impl<float, CharT, true>
    : public float_formatter<float, CharT> {};
template <class CharT>
struct formatter_impl<double, CharT, true>
    : public float_formatter<double, CharT> {};
template <class CharT>
struct formatter_impl<long double, CharT, true>
    : public float_formatter<long double, CharT> {};

//////////////////////////////
// pointer formatters

struct ptr_spec_delegate {
    constexpr void set_defaults(std_format_spec_base& spec) const noexcept {
        if (is_defaulted(spec.align)) {
            spec.align = alignment_t::left;
        }
        if (is_defaulted(spec.type)) {
            spec.type = type_t::p;
        }
    }
    constexpr void verify(const std_format_spec_base& spec) const {
        if (spec.type != type_t::p)
            throw format_error("invalid format type for pointer");
        // TODO
    }
};

struct ptr_default_engine {
    const void* ptr;
    std::array<char, sizeof(void*) * 2 + 2> buf;
    std::size_t buf_size;

    ptr_default_engine(const void* p) : ptr{p} {
        nonoverlapping_str_writer writer;
        writer(std::string_view("0x"), buf.data());
        const auto result =
              std::to_chars(buf.data() + 2, buf.data() + buf.size(),
                            reinterpret_cast<std::uintptr_t>(ptr), 16);
        buf_size = result.ptr - buf.data();
    }

    template <class Out>
    constexpr Out write_value(Out out) const {
        return nonoverlapping_str_writer{}(
              std::string_view(buf.data(), buf_size), out);
    }
};

struct ptr_spec_engine : ptr_default_engine {
    template <class Out, class CharT>
    constexpr Out write_left_padding(std::size_t width,
                                     alignment_t align,
                                     CharT fill,
                                     Out out) const {
        return simple_padding_engine{align}.write_left(width, buf_size, fill,
                                                       out);
    }
    template <class Out, class CharT>
    constexpr Out write_right_padding(std::size_t width,
                                      alignment_t align,
                                      CharT fill,
                                      Out out) const {
        return simple_padding_engine{align}.write_right(width, buf_size, fill,
                                                        out);
    }
};

template <class Pointer, class CharT>
struct pointer_formatter : public std_formatter_driver<CharT> {
    using base = std_formatter_driver<CharT>;
    template <typename Out>
    constexpr typename basic_format_context<Out, CharT>::iterator format(
          Pointer p,
          basic_format_context<Out, CharT>& fc) {
        if (!base::spec) {
            fc.advance_to(ptr_default_engine{p}.write_value(fc.out()));
            return fc.out();
        }
        base::finalize_spec(ptr_spec_delegate{});
        fc.advance_to(base::format_to_spec(fc, ptr_spec_engine{{p}}));
        return fc.out();
    }
};
template <class CharT>
struct formatter_impl<std::nullptr_t, CharT, true>
    : public pointer_formatter<std::nullptr_t, CharT> {};
template <class CharT>
struct formatter_impl<void*, CharT, true>
    : public pointer_formatter<void*, CharT> {};
template <class CharT>
struct formatter_impl<const void*, CharT, true>
    : public pointer_formatter<const void*, CharT> {};

//////////////////////////////
// string formatters

struct str_spec_delegate {
    constexpr void set_defaults(std_format_spec_base& spec) const noexcept {
        if (is_defaulted(spec.align)) {
            spec.align = alignment_t::left;
        }
        if (is_defaulted(spec.type)) {
            spec.type = type_t::s;
        }
    }
    constexpr void verify(const std_format_spec_base&) const {
        // TODO
    }
};

template <class CharT, class Traits>
struct str_default_engine {
    std::basic_string_view<CharT, Traits> str;
    template <class Out>
    constexpr Out write_value(Out out) const {
        return overlapping_str_writer{}(str, out);
    }
};

template <class CharT, class Traits>
struct str_spec_engine : str_default_engine<CharT, Traits> {
    using base = str_default_engine<CharT, Traits>;

    template <class Out>
    constexpr Out write_left_padding(std::size_t width,
                                     alignment_t align,
                                     CharT fill,
                                     Out out) const {
        return simple_padding_engine{align}.write_left(width, base::str.size(),
                                                       fill, out);
    }
    template <class Out>
    constexpr Out write_right_padding(std::size_t width,
                                      alignment_t align,
                                      CharT fill,
                                      Out out) const {
        return simple_padding_engine{align}.write_right(width, base::str.size(),
                                                        fill, out);
    }
};

template <class CharT>
struct str_formatter_base : public std_formatter_driver<CharT> {
    using base = std_formatter_driver<CharT>;

   protected:
    template <typename Out>
    constexpr Out format_impl(basic_string_view<CharT> s,
                              basic_format_context<Out, CharT>& fc) {
        using Traits = typename basic_string_view<CharT>::traits_type;
        if (!base::spec) {
            fc.advance_to(
                  str_default_engine<CharT, Traits>{s}.write_value(fc.out()));
            return fc.out();
        }
        base::finalize_spec(str_spec_delegate{});
        fc.advance_to(
              base::format_to_spec(fc, str_spec_engine<CharT, Traits>{{s}}));
        return fc.out();
    }
};

template <class T>
struct is_string : std::false_type {};
template <class CharT, class Traits, class Alloc>
struct is_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {};

template <class Str, class CharT>
struct str_formatter : public str_formatter_base<CharT> {
    using base = str_formatter_base<CharT>;
    template <typename Out>
    constexpr Out format(std::conditional_t<is_string<Str>::value,
                                            const Str&,
                                            std::decay_t<Str>> i,
                         basic_format_context<Out, CharT>& fc) {
        return base::format_impl(i, fc);
    }
};

template <class CharT>
struct formatter_impl<CharT*, CharT, true>
    : public str_formatter<CharT*, CharT> {};
template <class CharT>
struct formatter_impl<const CharT*, CharT, true>
    : public str_formatter<const CharT*, CharT> {};
template <class CharT, std::size_t N>
struct formatter_impl<const CharT[N], CharT, true>
    : public str_formatter<const CharT[N], CharT> {};
template <class CharT, class Traits, class Alloc>
struct formatter_impl<std::basic_string<CharT, Traits, Alloc>, CharT, true>
    : public str_formatter<std::basic_string<CharT, Traits, Alloc>, CharT> {};
template <class CharT, class Traits>
struct formatter_impl<std::basic_string_view<CharT, Traits>, CharT, true>
    : public str_formatter<std::basic_string_view<CharT, Traits>, CharT> {};

}  // namespace detail

template <typename T, typename Char = char>
struct formatter : public detail::formatter_impl<T, Char> {};

namespace detail {

template <class CharT>
struct replacement_field {
    range<CharT> format_spec;
    arg_id_t arg_id;
};

template <class CharT>
struct fmt_str_parser {
    range<CharT> fmt;

    template <class Callbacks>
    constexpr bool parse(Callbacks cb) {
        auto write_text = [&](range<CharT> text) {
            while (!text.empty()) {
                auto rbrace_it = text.find('}');
                if (rbrace_it == text.end()) {
                    cb.text(text);
                    break;
                }
                ++rbrace_it;
                if (rbrace_it == text.end() || *rbrace_it != '}')
                    return false;
                cb.text(range<CharT>{text.begin(), rbrace_it});
                text.advance_to(rbrace_it + 1);
            }
            return true;
        };

        while (!fmt.empty()) {
            const auto lbrace_it = fmt.find('{');
            if (lbrace_it == fmt.end()) {
                return write_text(fmt);
            }
            auto next = lbrace_it + 1;
            if (next == fmt.end())
                return false;
            if (*next == '{') {
                write_text(range<CharT>{fmt.begin(), next});
                fmt.advance_to(next + 1);
            } else {
                write_text(range<CharT>{fmt.begin(), lbrace_it});
                fmt.advance_to(next);
                if (!parse_replacement_field(cb))
                    return false;
            }
        }
        return true;
    }

   private:
    constexpr parse_integer_result parse_arg_id() {
        if (fmt.consume('0'))
            return 0;
        return parse_integer(fmt);
    }

    template <class Callbacks>
    constexpr bool parse_replacement_field(Callbacks cb) {
        if (fmt.consume('}')) {
            cb.replacement_field(
                  replacement_field<CharT>{{}, arg_id_t::auto_id()});
            return true;
        }
        const parse_integer_result arg_value = parse_arg_id();
        if (arg_value.tag == parse_integer_result::type::error)
            return false;
        const arg_id_t arg_id =
              arg_value.tag == parse_integer_result::type::none
                    ? arg_id_t::auto_id()
                    : arg_id_t{arg_value.integer};
        if (fmt.consume(':')) {
            auto format_spec_end = find_balanced_delimiter_end(fmt, '{', '}');
            range<CharT> format_spec(fmt.begin(), format_spec_end);
            cb.replacement_field(replacement_field<CharT>{format_spec, arg_id});
            fmt.advance_to(format_spec_end);
        } else
            cb.replacement_field(replacement_field<CharT>{{}, arg_id});
        return fmt.consume('}');
    }
};

struct throw_uninitialized_format_arg {
    void operator()(std::monostate) const {
        throw format_error("uninitialized format argument");
    }
};

template <class Context, class CharT>
struct arg_out_func : throw_uninitialized_format_arg {
    using throw_uninitialized_format_arg::operator();

    template <class Arg>
    constexpr void impl(Arg val) const {
        typename Context::template formatter_type<Arg> f;
        pc.advance_to(f.parse(pc));
        fc.advance_to(f.format(val, fc));
    }
    template <class Arg>
    constexpr void operator()(Arg arg) const {
        impl(arg);
    }
    constexpr void operator()(int i) const {
        return impl(static_cast<long long>(i));
    }
    constexpr void operator()(unsigned int ui) const {
        return impl(static_cast<unsigned long long>(ui));
    }
    constexpr void operator()(const CharT* cptr) const {
        return impl(basic_string_view<CharT>(cptr));
    }

    constexpr void operator()(
          const typename basic_format_arg<Context>::handle handle) const {
        handle.format(pc, fc);
    }

    Context& fc;
    basic_format_parse_context<CharT>& pc;
};

template <class Context, class CharT>
constexpr void arg_out(Context& fc,
                       basic_format_parse_context<CharT>& pc,
                       const basic_format_arg<Context> arg) {
    visit_format_arg(arg_out_func<Context, CharT>{{}, fc, pc}, arg);
}

template <class CharT, class Out>
LRSTD_EXTRA_CONSTEXPR Out vformat_to_impl(Out out,
                                          basic_string_view<CharT> fmt_sv,
                                          format_args_t<Out, CharT>& args) {
    basic_format_context<Out, CharT> context(args, out);
    basic_format_parse_context<CharT> parse_context(fmt_sv,
                                                    context.args_size());

    range<CharT> fmt(fmt_sv);

    struct Callbacks {
        constexpr void text(range<CharT> range) const {
            context.advance_to(overlapping_str_writer{}(range.as_string_view(),
                                                        context.out()));
        }
        constexpr void replacement_field(replacement_field<CharT> field) const {
            parse_context._begin = field.format_spec.begin();
            parse_context._end = field.format_spec.end();
            arg_out(context, parse_context,
                    context.arg(
                          field.arg_id.is_auto()
                                ? parse_context.next_arg_id()
                                : (parse_context.check_arg_id(field.arg_id._id),
                                   field.arg_id._id)));
        }
        basic_format_context<Out, CharT>& context;
        basic_format_parse_context<CharT>& parse_context;
    };
    if (!fmt_str_parser<CharT>{{fmt}}.parse(
              Callbacks{context, parse_context})) {
        throw format_error("invalid format string");
    }
    return context.out();
}

template <class CharT>
LRSTD_EXTRA_CONSTEXPR std::basic_string<CharT> vformat_impl(
      basic_string_view<CharT> fmt,
      format_args_t<std::back_insert_iterator<std::basic_string<CharT>>, CharT>
            args) {
    std::basic_string<CharT> ret;
    vformat_to_impl(std::back_inserter(ret), fmt, args);
    return ret;
}

}  // namespace detail

template <class Out>
LRSTD_EXTRA_CONSTEXPR Out vformat_to(Out out,
                                     std::string_view fmt,
                                     format_args_t<Out, char> args) {
    return detail::vformat_to_impl(out, fmt, args);
}

template <class Out>
LRSTD_EXTRA_CONSTEXPR Out vformat_to(Out out,
                                     std::wstring_view fmt,
                                     format_args_t<Out, wchar_t> args) {
    return detail::vformat_to_impl(out, fmt, args);
}

template <class Out, class... Args>
LRSTD_EXTRA_CONSTEXPR Out format_to(Out out,
                                    std::string_view fmt,
                                    const Args&... args) {
    using Context = basic_format_context<Out, std::string_view::value_type>;
    return vformat_to(out, fmt, {make_format_args<Context>(args...)});
}

template <class Out, class... Args>
LRSTD_EXTRA_CONSTEXPR Out format_to(Out out,
                                    std::wstring_view fmt,
                                    const Args&... args) {
    using Context = basic_format_context<Out, std::wstring_view::value_type>;
    return vformat_to(out, fmt, {make_format_args<Context>(args...)});
}

inline std::string vformat(std::string_view fmt, format_args args) {
    return detail::vformat_impl(fmt, args);
}
inline std::wstring vformat(std::wstring_view fmt, wformat_args args) {
    return detail::vformat_impl(fmt, args);
}

template <class... Args>
std::string format(std::string_view fmt, const Args&... args) {
    return vformat(fmt, {make_format_args(args...)});
}

template <class... Args>
std::wstring format(std::wstring_view fmt, const Args&... args) {
    return vformat(fmt, {make_wformat_args(args...)});
}

template <class... Args>
LRSTD_EXTRA_CONSTEXPR std::size_t formatted_size(std::string_view fmt,
                                                 const Args&... args) {
    using Context = basic_format_context<detail::char_counter,
                                         std::string_view::value_type>;
    return vformat_to(detail::char_counter{}, fmt,
                      {make_format_args<Context>(args...)})
          .count;
}

template <class... Args>
LRSTD_EXTRA_CONSTEXPR std::size_t formatted_size(std::wstring_view fmt,
                                                 const Args&... args) {
    using Context = basic_format_context<detail::char_counter,
                                         std::wstring_view::value_type>;
    return vformat_to(detail::char_counter{}, fmt,
                      {make_format_args<Context>(args...)})
          .count;
}

template <class Out>
struct format_to_n_result {
    Out out;
    iter_difference_t<Out> size;
};

template <class Out, class... Args>
LRSTD_EXTRA_CONSTEXPR format_to_n_result<Out> format_to_n(
      Out out,
      iter_difference_t<Out> n,
      std::string_view fmt,
      const Args&... args) {
    auto result = format_to(detail::write_n_wrapper<Out>{n, out}, fmt, args...);
    return format_to_n_result<Out>{result.it, result.current + result.overflow};
}

template <class Out, class... Args>
LRSTD_EXTRA_CONSTEXPR format_to_n_result<Out> format_to_n(
      Out out,
      iter_difference_t<Out> n,
      std::wstring_view fmt,
      const Args&... args) {
    auto result = format_to(detail::write_n_wrapper<Out>{n, out}, fmt, args...);
    return format_to_n_result<Out>{result.it, result.current + result.overflow};
}

}  // namespace lrstd

#endif
