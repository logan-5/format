#ifndef LRSTD_FORMAT_HPP
#define LRSTD_FORMAT_HPP

#include <array>
#include <cassert>
#include <charconv>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>

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

namespace parse_utils {

template <class CharT>
struct range {
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
      CharT start,
      CharT end,
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

}  // namespace parse_utils

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

namespace std_format_spec_types {

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

}  // namespace std_format_spec_types

namespace parse_std_format_spec {

using namespace parse_utils;
using namespace std_format_spec_types;

template <class CharT>
struct std_format_spec {
    CharT fill{' '};
    alignment_t align{alignment_t::defaulted};
    bool alternate = false;
    sign_t sign{sign_t::minus};
    width_t width;
    integer_or_arg_id precision;
    type_t type{type_t::defaulted};
};

template <class CharT>
constexpr std::optional<alignment_t> parse_align(range<CharT>& fmt) noexcept {
    if (fmt.consume(static_cast<CharT>('<'))) {
        return alignment_t::left;
    }
    if (fmt.consume(static_cast<CharT>('>'))) {
        return alignment_t::right;
    }
    if (fmt.consume(static_cast<CharT>('^'))) {
        return alignment_t::center;
    }
    return std::nullopt;
}

template <class CharT>
constexpr std::optional<std::pair<CharT, alignment_t>> parse_fill_and_align(
      range<CharT>& fmt) {
    LRSTD_ASSERT(!fmt.empty());
    if (fmt.match_nonempty(static_cast<CharT>('{')) ||
        fmt.match_nonempty(static_cast<CharT>('}')))
        return std::nullopt;
    if (fmt.size() >= 2) {
        auto next = fmt.substr(1);
        if (const auto align = parse_align(next)) {
            const CharT fill = fmt[0];
            fmt = next;
            return std::make_pair(fill, *align);
        }
    }
    if (const auto align = parse_align(fmt)) {
        return std::make_pair(' ', *align);
    }
    return std::nullopt;
}

template <class CharT>
constexpr std::optional<sign_t> parse_sign(range<CharT>& fmt) {
    if (fmt.consume(static_cast<CharT>('+')))
        return sign_t::plus;
    if (fmt.consume(static_cast<CharT>('-')))
        return sign_t::minus;
    if (fmt.consume(static_cast<CharT>(' ')))
        return sign_t::space;
    return std::nullopt;
}

template <bool AllowLeadingZero, class CharT>
constexpr std::optional<integer_or_arg_id> parse_integer_or_arg_id(
      range<CharT>& fmt,
      basic_format_parse_context<CharT>& parse_context) {
    if constexpr (!AllowLeadingZero) {
        if (fmt.match(static_cast<CharT>('0')))
            return std::nullopt;
    }
    if (fmt.match(static_cast<CharT>('{'))) {
        auto next = fmt.substr(1);
        if (const parse_integer_result result = parse_integer(next)) {
            if (next.consume(static_cast<CharT>('}'))) {
                fmt = next;
                return arg_id_t{result.integer};
            }
        } else if (next.consume(static_cast<CharT>('}'))) {
            fmt = next;
            return arg_id_t{parse_context.next_arg_id()};
        }
    }
    if (const parse_integer_result result = parse_integer(fmt)) {
        return result.integer;
    }
    return std::nullopt;
}

template <class CharT>
constexpr std::optional<integer_or_arg_id> parse_precision(
      range<CharT>& fmt,
      basic_format_parse_context<CharT>& parse_context) {
    if (fmt.match(static_cast<CharT>('.'))) {
        auto next = fmt.substr(1);
        if (auto result = parse_integer_or_arg_id<true>(next, parse_context)) {
            fmt = next;
            return *result;
        }
    }
    return std::nullopt;
}

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
constexpr std::optional<type_t> parse_type(range<CharT>& fmt) {
    for (CharT c : type_chars<CharT>::value) {
        if (fmt.consume(c)) {
            return type_t(c);
        }
    }
    return std::nullopt;
}

template <class CharT>
constexpr std_format_spec<CharT> parse(
      basic_format_parse_context<CharT>& parse_context) {
    std_format_spec<CharT> r;

    range<CharT> fmt(parse_context.begin(), parse_context.end());

    if (fmt.empty())
        return r;

    if (const auto fill_and_align = parse_fill_and_align(fmt)) {
        r.fill = fill_and_align->first;
        r.align = fill_and_align->second;
    }
    if (const auto sign = parse_sign(fmt)) {
        r.sign = *sign;
    }
    if (fmt.consume(static_cast<CharT>('#'))) {
        r.alternate = true;
    }
    const bool width_leading_zero = fmt.consume(static_cast<CharT>('0'));
    if (const auto width = parse_integer_or_arg_id<false>(fmt, parse_context)) {
        r.width = width_t{*width, width_leading_zero};
    }
    if (const auto type = parse_type(fmt)) {
        r.type = *type;
    }
    parse_context.advance_to(fmt.begin());
    return r;
}

}  // namespace parse_std_format_spec

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
using overlapping_generic_writer = overloaded<overlapping_str_writer,
                                              single_char_writer,
                                              repeated_char_writer>;

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
using nonoverlapping_generic_writer = overloaded<nonoverlapping_str_writer,
                                                 single_char_writer,
                                                 repeated_char_writer>;

struct sign_char {
    constexpr char operator()(std_format_spec_types::sign_t sign,
                              bool is_negative) const noexcept {
        using S = std_format_spec_types::sign_t;
        switch (sign) {
            case S::plus:
                return is_negative ? '-' : '+';
            case S::minus:
                return is_negative ? '-' : '\0';
            case S::space:
                return is_negative ? '-' : ' ';
        }
        LRSTD_UNREACHABLE();
    }
};

namespace fst = std_format_spec_types;

template <class CharT>
struct std_format_parser {
    parse_std_format_spec::std_format_spec<CharT> format_spec;
    constexpr typename basic_format_parse_context<CharT>::iterator parse(
          basic_format_parse_context<CharT>& pc) {
        format_spec = parse_std_format_spec::parse(pc);
        if (pc.begin() != pc.end())
            throw format_error("bad standard format spec string");
        return pc.begin();
    }

    template <typename Out>
    constexpr std::size_t get_width(basic_format_context<Out, CharT>& fc) {
        using I = parse_std_format_spec::integer_or_arg_id;
        switch (format_spec.width.i.tag) {
            case I::which::integer:
                return format_spec.width.i.integer;
            case I::which::arg_id:
                return visit_format_arg(
                      [](auto arg) noexcept(std::is_integral_v<decltype(arg)>)
                            ->std::size_t {
                                if constexpr (std::is_integral_v<decltype(
                                                    arg)>) {
                                    return arg;
                                } else {
                                    throw format_error("width is not integral");
                                }
                            },
                      fc.arg(format_spec.width.i.arg_id._id));
        }
    }

    template <class Out>
    constexpr Out write_prefix(Out out,
                               std::size_t width,
                               const std::size_t value_width,
                               const basic_string_view<char> prefix,
                               const std_format_spec_types::alignment_t align,
                               const bool zero_pad,
                               const CharT sign_char) const noexcept {
        const bool has_sign = sign_char != '\0';
        nonoverlapping_generic_writer writer;
        auto write_sign_and_prefix = [&] {
            return writer(prefix, has_sign ? writer(sign_char, out) : out);
        };

        using namespace std_format_spec_types;
        if (!zero_pad && align == alignment_t::left) {
            return write_sign_and_prefix();
        }
        const std::size_t sign_width = static_cast<std::size_t>(has_sign);
        const std::size_t prefix_width = prefix.size() + sign_width;
        if (zero_pad) {
            out = write_sign_and_prefix();
            width = width > prefix_width ? width - prefix_width : 0;
            const auto zero_count =
                  width > value_width ? width - value_width : 0;
            return writer(static_cast<CharT>('0'), zero_count, out);
        }

        LRSTD_ASSERT(align == alignment_t::right ||
                     align == alignment_t::center);
        width = width > prefix_width ? width - prefix_width : 0;
        if (width > value_width) {
            const auto padding_chars = width - value_width;
            const auto fill_count = align == alignment_t::right
                                          ? padding_chars
                                          : padding_chars / 2;
            out = writer(format_spec.fill, fill_count, out);
        }
        return write_sign_and_prefix();
    }
    template <typename Out>
    constexpr Out write_suffix(Out out,
                               std::size_t width,
                               std::size_t value_and_prefix_width,
                               std_format_spec_types::alignment_t align,
                               bool zero_pad) const {
        if (zero_pad || value_and_prefix_width >= width)
            return out;

        using namespace std_format_spec_types;
        switch (align) {
            case alignment_t::center: {
                const auto padding_chars = width - value_and_prefix_width;
                return repeated_char_writer{}(
                      format_spec.fill, padding_chars / 2 + (padding_chars & 1),
                      out);
            }
            case alignment_t::left: {
                const auto padding_chars = width - value_and_prefix_width;
                return repeated_char_writer{}(format_spec.fill, padding_chars,
                                              out);
            }
            case alignment_t::right:
                return out;
            case alignment_t::defaulted:
                break;
        }
        LRSTD_UNREACHABLE();
    }

    template <class Out, class FormatFunc, class Writer>
    constexpr Out do_format(Out out,
                            const std::size_t width,
                            FormatFunc&& f,
                            Writer writer) {
        using namespace std_format_spec_types;
        const type_t type = is_defaulted(format_spec.type) ? f.default_type()
                                                           : format_spec.type;
        const bool default_align = is_defaulted(format_spec.align);
        const bool zero_pad = default_align && format_spec.width.zero_pad;
        if (zero_pad) {
            f.validate_zero_pad(type);
        }
        const alignment_t align =
              default_align ? f.default_alignment(type) : format_spec.align;
        const char sign = sign_char{}(format_spec.sign, f.is_negative());
        if (sign != '\0') {
            f.validate_sign(type);
        }
        if (type == type_t::c) {
            return do_char_format(out, width, f, writer, align);
        }
        return do_str_format(out, width, f, writer, sign, align, type,
                             zero_pad);
    }

    template <class Out, class FormatFunc, class Writer>
    constexpr Out do_str_format(Out out,
                                const std::size_t width,
                                FormatFunc&& f,
                                Writer writer,
                                char sign,
                                fst::alignment_t align,
                                fst::type_t type,
                                bool zero_pad) {
        const auto formatted_str = f.formatted_str(type, format_spec.alternate);
        const std::size_t value_width = formatted_str.size();
        const std::string_view prefix =
              f.get_prefix(type, format_spec.alternate);
        out = write_prefix(out, width, value_width, prefix, align, zero_pad,
                           sign);
        out = writer(formatted_str, out);
        const std::size_t value_and_prefix_width =
              value_width + prefix.size() + (sign != '\0');
        out = write_suffix(out, width, value_and_prefix_width, align, zero_pad);
        return out;
    }

    template <class Out, class FormatFunc, class Writer>
    constexpr Out do_char_format(Out out,
                                 const std::size_t width,
                                 FormatFunc&& f,
                                 Writer writer,
                                 fst::alignment_t align) {
        const auto formatted_char = f.formatted_char();
        const std::size_t value_width = 1;
        const std::string_view prefix;
        out = write_prefix(out, width, value_width, prefix, align, false, '\0');
        out = writer(formatted_char, out);
        const std::size_t value_and_prefix_width = value_width;
        out = write_suffix(out, width, value_and_prefix_width, align, false);
        return out;
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

template <class Int>
struct format_int_common {
    typename format_int_storage_type<Int>::type i;
    std::array<char, sizeof(Int) * 8> buf;

    constexpr std::string_view formatted_str(std_format_spec_types::type_t type,
                                             bool) {
        auto to_chars = [this](int base) -> std::string_view {
            const auto result =
                  std::to_chars(std::begin(buf), std::end(buf), i, base);
            LRSTD_ASSERT(result.ec == std::errc());
            const auto start =
                  std::next(std::begin(buf),
                            is_negative());  // skip minus sign, if any.
                                             // already taken care of
            return std::string_view(start, std::distance(start, result.ptr));
        };
        using T = std_format_spec_types::type_t;
        switch (type) {
            case T::x:
                return to_chars(16);
            case T::X: {
                auto result = to_chars(16);
                convert_to_upper();
                return result;
            }
            case T::b:
                return to_chars(2);
            case T::B:
                return to_chars(2);
            case T::d:
                return to_chars(10);
            case T::o:
                return to_chars(8);

            case T::n:
                throw "TODO";

            case T::s:
                if constexpr (std::is_same_v<Int, bool>) {
                    const std::string_view s =
                          static_cast<bool>(i) ? "true" : "false";
                    nonoverlapping_str_writer{}(s, buf.data());
                    return std::string_view(std::begin(buf), s.size());
                }
                [[fallthrough]];

                // clang-format off
            case T::a: case T::A: case T::e: case T::E: case T::f: 
            case T::F: case T::g: case T::G: case T::p:
                // clang-format on
                throw format_error("invalid formatting type for integer");

            case T::c:
            case T::defaulted:
                break;
        }
        LRSTD_UNREACHABLE();
    }

    static constexpr bool is_char_or_bool() noexcept {
        return is_char_or_wchar_v<Int> || std::is_same_v<Int, bool>;
    }

    static constexpr std_format_spec_types::type_t default_type() noexcept {
        if constexpr (is_char_or_wchar_v<Int>) {
            return std_format_spec_types::type_t::c;
        } else if constexpr (std::is_same_v<Int, bool>) {
            return std_format_spec_types::type_t::s;
        } else {
            return std_format_spec_types::type_t::d;
        }
    }
    static constexpr std_format_spec_types::alignment_t default_alignment(
          std_format_spec_types::type_t type) noexcept {
        using A = std_format_spec_types::alignment_t;
        if constexpr (is_char_or_bool()) {
            return is_integer_type(type) ? A::right : A::left;
        } else {
            return A::right;
        }
    }
    constexpr bool is_negative() const noexcept { return i < 0; }
    static constexpr void validate_sign(
          std_format_spec_types::type_t type) noexcept(!is_char_or_bool()) {
        if constexpr (is_char_or_bool()) {
            if (!is_integer_type(type))
                throw format_error("illegal sign option for type");
        }
    }
    static constexpr void validate_zero_pad(
          std_format_spec_types::type_t type) noexcept(!is_char_or_bool()) {
        if constexpr (is_char_or_bool()) {
            if (!is_integer_type(type))
                throw format_error("illegal zero padding option for type");
        }
    }

    static constexpr basic_string_view<char> get_prefix(
          std_format_spec_types::type_t type,
          bool alternate) noexcept {
        if (!alternate)
            return {};
        using T = std_format_spec_types::type_t;
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

   private:
    constexpr void convert_to_upper() {
        std::transform(std::begin(buf), std::end(buf), std::begin(buf),
                       [](char c) { return std::toupper(c); });
    }

    static constexpr bool is_integer_type(
          std_format_spec_types::type_t type) noexcept {
        constexpr std::string_view types = "xXbBdon";
        return types.find(static_cast<char>(type)) != std::string_view::npos;
    }
};

template <class Int, class CharT>
struct format_int : public format_int_common<Int> {
    constexpr CharT formatted_char() const {
        if (!representable_as_char<CharT>(this->i))
            throw format_error("integer is not in representable range of char");
        return static_cast<CharT>(this->i);
    }
};

template <class Int, class CharT>
struct int_formatter : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    constexpr typename basic_format_context<Out, CharT>::iterator format(
          Int i,
          basic_format_context<Out, CharT>& fc) {
        fc.advance_to(to_iter(
              base::do_format(maybe_to_raw_pointer(fc.out()),
                              base::get_width(fc), format_int<Int, CharT>{{i}},
                              nonoverlapping_generic_writer{}),
              fc.out()));
        return fc.out();
    }
};

template <class CharT>
struct formatter_impl<signed char, CharT, true>
    : public int_formatter<signed char, CharT> {};
template <class CharT>
struct formatter_impl<unsigned char, CharT, true>
    : public int_formatter<unsigned char, CharT> {};
template <class CharT>
struct formatter_impl<short int, CharT, true>
    : public int_formatter<short int, CharT> {};
template <class CharT>
struct formatter_impl<unsigned short int, CharT, true>
    : public int_formatter<unsigned short int, CharT> {};
template <class CharT>
struct formatter_impl<int, CharT, true> : public int_formatter<int, CharT> {};
template <class CharT>
struct formatter_impl<unsigned int, CharT, true>
    : public int_formatter<unsigned int, CharT> {};
template <class CharT>
struct formatter_impl<long int, CharT, true>
    : public int_formatter<long int, CharT> {};
template <class CharT>
struct formatter_impl<unsigned long int, CharT, true>
    : public int_formatter<unsigned long int, CharT> {};
template <class CharT>
struct formatter_impl<long long int, CharT, true>
    : public int_formatter<long long int, CharT> {};
template <class CharT>
struct formatter_impl<unsigned long long int, CharT, true>
    : public int_formatter<unsigned long long int, CharT> {};

template <class CharT>
struct formatter_impl<bool, CharT, true> : public int_formatter<bool, CharT> {};

template <class CharT>
struct formatter_impl<CharT, CharT, true>
    : public int_formatter<CharT, CharT> {};
template <>
struct formatter_impl<char, wchar_t, true>
    : public int_formatter<char, wchar_t> {};

//////////////////////////////
// float formatters

template <class Float, class CharT>
struct float_formatter : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    constexpr typename basic_format_context<Out, CharT>::iterator format(
          Float,
          basic_format_context<Out, CharT>&) {
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

struct format_pointer {
    const void* ptr;
    std::array<char, sizeof(void*) * 2 + 2> buf;

    char formatted_char() const { LRSTD_UNREACHABLE(); }
    constexpr std::string_view formatted_str(std_format_spec_types::type_t type,
                                             bool alternate) {
        if (alternate)
            throw format_error("invalid '#' option for formatting pointer");
        if (type != std_format_spec_types::type_t::p &&
            type != std_format_spec_types::type_t::defaulted)
            throw format_error("invalid type spec for formatting pointer");

        nonoverlapping_str_writer{}(std::string_view("0x"), buf.data());
        const auto result =
              std::to_chars(buf.data() + 2, buf.data() + buf.size(),
                            reinterpret_cast<std::uintptr_t>(ptr), 16);
        LRSTD_ASSERT(result.ec == std::errc());
        return std::string_view(buf.data(),
                                std::distance(buf.data(), result.ptr));
    }

    static constexpr std_format_spec_types::type_t default_type() noexcept {
        return std_format_spec_types::type_t::p;
    }
    static constexpr std_format_spec_types::alignment_t default_alignment(
          std_format_spec_types::type_t) noexcept {
        return std_format_spec_types::alignment_t::left;
    }
    constexpr bool is_negative() const noexcept { return false; }
    static void validate_sign(std_format_spec_types::type_t) noexcept(false) {
        throw format_error("sign option invalid for pointers");
    }
    static void validate_zero_pad(std_format_spec_types::type_t) noexcept(
          false) {
        throw format_error("zero padding invalid for pointers");
    }
    static constexpr basic_string_view<char> get_prefix(
          std_format_spec_types::type_t,
          bool) noexcept {
        return {};
    }
};

template <class Pointer, class CharT>
struct pointer_formatter : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    constexpr typename basic_format_context<Out, CharT>::iterator format(
          Pointer p,
          basic_format_context<Out, CharT>& fc) {
        fc.advance_to(to_iter(
              base::do_format(maybe_to_raw_pointer(fc.out()),
                              base::get_width(fc),
                              format_pointer{static_cast<const void*>(p)},
                              nonoverlapping_generic_writer{}),
              fc.out()));
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

template <class CharT, class Traits>
struct format_str {
    std::basic_string_view<CharT, Traits> s;

    CharT formatted_char() const { LRSTD_UNREACHABLE(); }
    constexpr std::basic_string_view<CharT, Traits> formatted_str(
          std_format_spec_types::type_t type,
          bool alternate) const {
        if (alternate)
            throw format_error("invalid '#' option for formatting string");
        if (type != std_format_spec_types::type_t::s &&
            type != std_format_spec_types::type_t::defaulted)
            throw format_error("invalid type spec for formatting string");
        return s;
    }

    static constexpr std_format_spec_types::type_t default_type() noexcept {
        return std_format_spec_types::type_t::s;
    }
    static constexpr std_format_spec_types::alignment_t default_alignment(
          std_format_spec_types::type_t) noexcept {
        return std_format_spec_types::alignment_t::left;
    }
    constexpr bool is_negative() const noexcept { return false; }
    static void validate_sign(std_format_spec_types::type_t) noexcept(false) {
        throw format_error("sign option invalid for strings");
    }
    static void validate_zero_pad(std_format_spec_types::type_t) noexcept(
          false) {
        throw format_error("zero padding invalid for strings");
    }
    static constexpr basic_string_view<char> get_prefix(
          std_format_spec_types::type_t,
          bool) noexcept {
        return {};
    }
};

template <class Str, class CharT>
struct str_formatter : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    constexpr typename basic_format_context<Out, CharT>::iterator format(
          std::decay_t<Str> i,
          basic_format_context<Out, CharT>& fc) {
        using Traits = std::char_traits<CharT>;
        fc.advance_to(to_iter(
              base::do_format(maybe_to_raw_pointer(fc.out()),
                              base::get_width(fc), format_str<CharT, Traits>{i},
                              overlapping_generic_writer{}),
              fc.out()));
        return fc.out();
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

namespace parse_fmt_str {

using namespace parse_utils;

template <class CharT>
struct text {
    range<CharT> text;
};

template <class CharT>
struct replacement_field {
    range<CharT> format_spec;
    arg_id_t arg_id;
};

enum class result_type : char { end, error, text, replacement_field };
template <class CharT>
struct result {
    union {
        std::monostate _;
        text<CharT> txt;
        replacement_field<CharT> repl;
    };
    result_type tag;
    constexpr result(result_type type) noexcept : _{}, tag{type} {}
    constexpr result(text<CharT> t) noexcept : txt{t}, tag{result_type::text} {}
    constexpr result(replacement_field<CharT> r) noexcept
        : repl{r}, tag{result_type::replacement_field} {}
};

template <class CharT>
constexpr parse_integer_result parse_arg_id(range<CharT>& fmt) {
    if (fmt.consume(static_cast<CharT>('0')))
        return 0;
    return parse_integer(fmt);
}

template <class CharT>
constexpr std::optional<replacement_field<CharT>> get_replacement_field(
      parse_integer_result i) {
    using Tag = parse_integer_result::type;
    switch (i.tag) {
        case Tag::error:
            return std::nullopt;
        case Tag::none:
            return replacement_field<CharT>{{}, arg_id_t::auto_id()};
        case Tag::success:
            return replacement_field<CharT>{{}, arg_id_t{i.integer}};
    }
}

template <class CharT>
constexpr std::optional<replacement_field<CharT>> parse_replacement_field(
      range<CharT>& fmt) {
    auto field = get_replacement_field<CharT>(parse_arg_id(fmt));
    if (!field)
        return std::nullopt;
    if (fmt.consume(static_cast<CharT>(':'))) {
        auto format_spec_end = find_balanced_delimiter_end(
              fmt, static_cast<CharT>('{'), static_cast<CharT>('}'));
        field->format_spec = range<CharT>(fmt.begin(), format_spec_end);
        fmt.advance_to(format_spec_end);
    }
    return field;
}

template <class CharT>
constexpr basic_string_view<CharT> lrbrace() noexcept {
    if constexpr (std::is_same_v<CharT, char>) {
        return "{}";
    } else if constexpr (std::is_same_v<CharT, wchar_t>) {
        return L"{}";
    }
}

template <class CharT, class Callbacks>
constexpr bool parse(range<CharT> fmt, Callbacks cb) {
    while (!fmt.empty()) {
        const auto lbrace_it = fmt.find('{');
        const auto rbrace_it = fmt.find('}');
        if (lbrace_it == fmt.end() && rbrace_it == fmt.end()) {
            cb.text(fmt);
            break;
        }
        if (lbrace_it < rbrace_it) {
            auto next = lbrace_it + 1;
            if (next == fmt.end())
                return false;
            if (*next == '{') {
                range<CharT> text{fmt.begin(), next};
                fmt.advance_to(next + 1);
                cb.text(text);
            } else if (*next == '}') {
                range<CharT> text{fmt.begin(), lbrace_it};
                fmt.advance_to(next + 1);
                cb.text(text);
                cb.replacement_field(
                      replacement_field<CharT>{{}, arg_id_t::auto_id()});
            } else {
                range<CharT> text{fmt.begin(), lbrace_it};
                fmt.advance_to(next);
                cb.text(text);
                const auto result = parse_replacement_field(fmt);
                if (!result || !fmt.consume(static_cast<CharT>('}')))
                    return false;
                cb.replacement_field(*result);
            }
        } else {
            auto next = rbrace_it + 1;
            if (next != fmt.end() && *next == '}') {
                range<CharT> text{fmt.begin(), next};
                fmt.advance_to(next + 1);
                cb.text(text);
            } else {
                return false;
            }
        }
    }
    return true;
}

}  // namespace parse_fmt_str

template <typename T, typename Ret = void>
struct throw_uninitialized_format_arg {
    Ret operator()(const T&) const {
        throw format_error("uninitialized format argument");
    }
};

template <class Context, class CharT>
constexpr void arg_out(Context& fc,
                       basic_format_parse_context<CharT>& pc,
                       const basic_format_arg<Context>& arg) {
    visit_format_arg(
          overloaded{
                throw_uninitialized_format_arg<std::monostate>{},
                [&](const typename basic_format_arg<Context>::handle handle) {
                    handle.format(pc, fc);
                },
                [&](const auto val) {
                    using T = std::remove_cv_t<
                          std::remove_reference_t<decltype(val)>>;
                    typename Context::template formatter_type<T> f;
                    pc.advance_to(f.parse(pc));
                    fc.advance_to(f.format(val, fc));
                }},
          arg);
}

template <typename T, typename Ret = void>
struct unreachable {
    Ret operator()(const T&) const { LRSTD_UNREACHABLE(); }
};

template <class CharT, class Out>
LRSTD_EXTRA_CONSTEXPR Out vformat_to_impl(Out out,
                                          basic_string_view<CharT> fmt_sv,
                                          format_args_t<Out, CharT>& args) {
    basic_format_context<Out, CharT> context(args, out);
    basic_format_parse_context<CharT> parse_context(fmt_sv,
                                                    context.args_size());

    namespace p = parse_fmt_str;
    p::range<CharT> fmt(fmt_sv);

    struct Callbacks {
        constexpr void text(p::range<CharT> range) const {
            context.advance_to(overlapping_str_writer{}(range.as_string_view(),
                                                        context.out()));
        }
        constexpr void replacement_field(
              p::replacement_field<CharT> field) const {
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
    if (!p::parse(fmt, Callbacks{context, parse_context})) {
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
