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
Out vformat_to_impl(Out out,
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
    friend O detail::vformat_to_impl(O out,
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
}  // namespace detail

namespace detail {
template <class Context, class... Args>
struct args_storage;
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
        explicit handle(const T& val) noexcept
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
        void format(basic_format_parse_context<char_type>& pc,
                    Context& fc) const {
            _format(pc, fc, _ptr);
        }
    };

   private:
    using char_type = typename Context::char_type;

    std::variant<std::monostate,
                 bool,
                 char_type,
                 int,
                 unsigned int,
                 long long int,
                 unsigned long long int,
                 double,
                 long double,
                 const char_type*,
                 basic_string_view<char_type>,
                 const void*,
                 handle>
          value;

    template <class T,
              typename = std::enable_if_t<std::is_same_v<
                    decltype(typename Context::template formatter_type<T>()
                                   .format(std::declval<const T&>(),
                                           std::declval<Context&>())),
                    typename Context::iterator>>>
    explicit basic_format_arg(const T& v) noexcept
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

    explicit basic_format_arg(float n) noexcept
        : value{static_cast<double>(n)} {}
    explicit basic_format_arg(double n) noexcept : value{n} {}
    explicit basic_format_arg(long double n) noexcept : value{n} {}
    explicit basic_format_arg(const char_type* s) : value{s} {}

    template <class Traits>
    explicit basic_format_arg(
          std::basic_string_view<char_type, Traits> s) noexcept
        : value{s} {}
    template <class Traits, class Alloc>
    explicit basic_format_arg(
          const std::basic_string<char_type, Traits, Alloc>& s)
        : value{basic_string_view<char_type>{s.data(), s.size()}} {}

    explicit basic_format_arg(std::nullptr_t)
        : value{static_cast<const void*>(nullptr)} {}

    template <class T, class = std::enable_if_t<std::is_void_v<T>>>
    explicit basic_format_arg(T* p) : value{p} {}

    template <class Ctx, class... Args>
    friend detail::args_storage<Ctx, Args...> make_format_args(const Args&...);

    template <class Visitor, class Ctx>
    friend auto visit_format_arg(Visitor&&, basic_format_arg<Ctx>);

   public:
    basic_format_arg() noexcept = default;

    explicit operator bool() const noexcept {
        return std::holds_alternative<std::monostate>(value);
    }
};

template <class Visitor, class Context>
auto visit_format_arg(Visitor&& visitor, basic_format_arg<Context> arg) {
    using CharT = typename basic_format_arg<Context>::char_type;
    // TODO
    if (auto* val = std::get_if<typename basic_format_arg<Context>::handle>(
              &arg.value))
        return visitor(*val);
    if (auto* val = std::get_if<CharT>(&arg.value))
        return visitor(*val);
    if (auto* val = std::get_if<std::monostate>(&arg.value))
        return visitor(*val);
    if (auto* val = std::get_if<int>(&arg.value))
        return visitor(*val);
    if (auto* val = std::get_if<bool>(&arg.value))
        return visitor(*val);
    if (auto* val = std::get_if<basic_string_view<CharT>>(&arg.value))
        return visitor(*val);
    if (auto* val = std::get_if<const CharT*>(&arg.value))
        return visitor(*val);
    throw;
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
    constexpr basic_format_args() noexcept : _size{0}, _data{nullptr} {}

    template <class... Args>
    basic_format_args(
          const detail::args_storage<Context, Args...>& storage) noexcept
        : _size{storage.args.size()}, _data{storage.args.data()} {}

    constexpr basic_format_arg<Context> get(std::size_t i) const noexcept {
        return i < _size ? _data[i] : basic_format_arg<Context>();
    }
};

template <class Out, class CharT>
class basic_format_context {
    basic_format_args<basic_format_context> _args;
    Out _out;

    basic_format_context(const basic_format_args<basic_format_context>& args,
                         Out out)
        : _args{args}, _out{out} {}

    template <class C, class O>
    friend O detail::vformat_to_impl(O out,
                                     basic_string_view<C> fmt,
                                     format_args_t<O, C>& args);

    constexpr std::size_t args_size() const { return _args._size; }

   public:
    using iterator = Out;
    using char_type = CharT;
    template <class T>
    using formatter_type = formatter<T, CharT>;

    basic_format_arg<basic_format_context> arg(std::size_t id_) const {
        return _args.get(id_);
    }

    iterator out() noexcept { return _out; }
    void advance_to(iterator it) { _out = it; }
};

template <class Context = format_context, class... Args>
detail::args_storage<Context, Args...> make_format_args(const Args&... args) {
    return {basic_format_arg<Context>(args)...};
}

template <class... Args>
detail::args_storage<wformat_context, Args...> make_wformat_args(
      const Args&... args) {
    return make_format_args<wformat_context>(args...);
}

namespace detail {

template <typename... F>
struct overloaded : public F... {
    using F::operator()...;
};
template <typename... F>
overloaded(F...)->overloaded<F...>;

namespace fmt_out {
template <class CharT, class Out>
Out text_out(basic_string_view<CharT> text, Out out);

template <class CharT, class Out>
Out chars_out(CharT c, std::size_t count, Out out);
}  // namespace fmt_out

namespace parse_utils {

template <class CharT>
constexpr bool match(basic_string_view<CharT> fmt, CharT c) {
    return !fmt.empty() && fmt[0] == c;
}
template <class CharT>
constexpr bool consume(basic_string_view<CharT>& fmt,
                       basic_string_view<CharT> prefix) {
    using Traits = typename basic_string_view<CharT>::traits_type;
    if (fmt.size() >= prefix.size() &&
        Traits::compare(fmt.begin(), prefix.begin(), prefix.size()) == 0) {
        fmt.remove_prefix(prefix.size());
        return true;
    }
    return false;
}
template <class CharT>
constexpr bool consume(basic_string_view<CharT>& fmt, CharT c) {
    if (match(fmt, c)) {
        fmt.remove_prefix(1);
        return true;
    }
    return false;
}
template <class CharT>
void advance_to(basic_string_view<CharT>& s,
                typename basic_string_view<CharT>::iterator pos) {
    s.remove_prefix(std::distance(s.begin(), pos));
}

struct error {};

inline std::variant<std::size_t, std::nullopt_t, error> parse_integer(
      basic_string_view<char>& fmt) {
    std::size_t val = 0;
    const auto result = std::from_chars(fmt.begin(), fmt.end(), val, 10);
    if (result.ptr == fmt.begin())
        return std::nullopt;
    if (result.ec != std::errc())
        return error{};
    advance_to(fmt, result.ptr);
    return val;
}

inline std::variant<std::size_t, std::nullopt_t, error> parse_integer(
      basic_string_view<wchar_t>&) {
    throw "not yet implemented";
}

template <class CharT>
typename basic_string_view<CharT>::iterator find(basic_string_view<CharT> fmt,
                                                 CharT c) {
    using Traits = typename basic_string_view<CharT>::traits_type;
    static_assert(
          std::is_same_v<typename basic_string_view<CharT>::const_iterator,
                         const CharT*>,
          "this won't work");
    const CharT* result = Traits::find(fmt.begin(), fmt.size(), c);
    return result ? result : fmt.end();
}

template <class CharT>
typename basic_string_view<CharT>::iterator find_balanced_delimiter_end(
      basic_string_view<CharT> fmt,
      CharT start,
      CharT end,
      std::size_t start_count = 1) {
    auto it = fmt.cbegin();
    for (; it != fmt.cend(); ++it) {
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
using integer_or_arg_id = std::variant<std::size_t, arg_id_t>;

enum class alignment_t : char {
    defaulted,
    left,
    right,
    center,
    after_sign,
};
inline constexpr bool is_defaulted(alignment_t a) noexcept {
    return a == alignment_t::defaulted;
}

enum class sign_t : char { plus, minus, space };
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
    sign_t sign{};
    width_t width;
    integer_or_arg_id precision;
    type_t type{type_t::defaulted};
};

inline std::string dump(const std_format_spec<char>& s) {
    using namespace std::string_literals;
    std::string ret = "format-spec{";
    ret += "fill: "s + s.fill;
    ret += ", align: ";
    switch (s.align) {
        case alignment_t::left:
            ret += "left";
            break;
        case alignment_t::right:
            ret += "right";
            break;
        case alignment_t::center:
            ret += "center";
            break;
        case alignment_t::after_sign:
            ret += "after space";
            break;
        default:
            ret += "invalid!!";
            break;
    }
    ret += ", alternate: "s + (s.alternate ? "true" : "false");
    ret += ", sign: ";
    switch (s.sign) {
        case sign_t::plus:
            ret += "plus";
            break;
        case sign_t::minus:
            ret += "minus";
            break;
        case sign_t::space:
            ret += "space";
            break;
        default:
            ret += "invalid!!";
            break;
    }
    ret += ", width: ("s +
           std::visit(
                 overloaded{[](std::size_t s) { return std::to_string(s); },
                            [](arg_id_t a) { return std::to_string(a._id); }},
                 s.width.i) +
           ", " + (s.width.zero_pad ? "true" : "false");
    ret += ", precision: "s +
           std::visit(
                 overloaded{[](std::size_t s) { return std::to_string(s); },
                            [](arg_id_t a) { return std::to_string(a._id); }},
                 s.precision);
    ret += ", type: "s + static_cast<char>(s.type) + "}";
    return ret;
}
inline std::wstring dump(const std_format_spec<wchar_t>&) {
    throw "not implemented";
}

template <class CharT>
constexpr std::optional<std::pair<CharT, alignment_t>> parse_fill_and_align(
      basic_string_view<CharT>& fmt) {
    if (fmt.size() < 2)
        return std::nullopt;
    const CharT fill = fmt[0];
    if (fill == static_cast<CharT>('{') || fill == static_cast<CharT>('}'))
        return std::nullopt;
    auto next = fmt.substr(1);
    auto advance = [&fmt] { fmt.remove_prefix(2); };
    if (match(next, static_cast<CharT>('<'))) {
        advance();
        return std::make_pair(fill, alignment_t::left);
    }
    if (match(next, static_cast<CharT>('>'))) {
        advance();
        return std::make_pair(fill, alignment_t::right);
    }
    if (match(next, static_cast<CharT>('='))) {
        advance();
        return std::make_pair(fill, alignment_t::after_sign);
    }
    if (match(next, static_cast<CharT>('^'))) {
        advance();
        return std::make_pair(fill, alignment_t::center);
    }
    return std::nullopt;
}

template <class CharT>
constexpr std::optional<sign_t> parse_sign(basic_string_view<CharT>& fmt) {
    if (consume(fmt, static_cast<CharT>('+')))
        return sign_t::plus;
    if (consume(fmt, static_cast<CharT>('-')))
        return sign_t::minus;
    if (consume(fmt, static_cast<CharT>(' ')))
        return sign_t::space;
    return std::nullopt;
}

template <bool AllowLeadingZero, class CharT>
constexpr std::optional<integer_or_arg_id> parse_integer_or_arg_id(
      basic_string_view<CharT>& fmt,
      basic_format_parse_context<CharT>& parse_context) {
    if constexpr (!AllowLeadingZero) {
        if (match(fmt, static_cast<CharT>('0')))
            return std::nullopt;
    }
    if (match(fmt, static_cast<CharT>('{'))) {
        auto next = fmt.substr(1);
        const auto integer = parse_integer(next);
        if (const auto* const i = std::get_if<std::size_t>(&integer)) {
            if (consume(next, static_cast<CharT>('}'))) {
                fmt = next;
                return arg_id_t{*i};
            }
        } else if (consume(next, static_cast<CharT>('}'))) {
            fmt = next;
            return arg_id_t{parse_context.next_arg_id()};
        }
    }
    const auto integer = parse_integer(fmt);
    if (const auto* const i = std::get_if<std::size_t>(&integer)) {
        return *i;
    }
    return std::nullopt;
}

template <class CharT>
constexpr std::optional<integer_or_arg_id> parse_precision(
      basic_string_view<CharT>& fmt,
      basic_format_parse_context<CharT>& parse_context) {
    if (match(fmt, '.')) {
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
constexpr std::optional<type_t> parse_type(basic_string_view<CharT>& fmt) {
    for (CharT c : type_chars<CharT>::value) {
        if (consume(fmt, c)) {
            return type_t(c);
        }
    }
    return std::nullopt;
}

template <class CharT>
constexpr std_format_spec<CharT> parse(
      basic_format_parse_context<CharT>& parse_context) {
    std_format_spec<CharT> r;

    basic_string_view<CharT> fmt(
          parse_context.begin(),
          std::distance(parse_context.begin(), parse_context.end()));
    if (fmt.empty())
        return r;

    if (const auto fill_and_align = parse_fill_and_align(fmt)) {
        std::tie(r.fill, r.align) = *fill_and_align;
    }
    if (const auto sign = parse_sign(fmt)) {
        r.sign = *sign;
    }
    if (consume(fmt, static_cast<CharT>('#'))) {
        r.alternate = true;
    }
    const bool width_leading_zero = consume(fmt, static_cast<CharT>('0'));
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

struct write_single_char {
    template <class CharT, class Out>
    constexpr Out operator()(CharT c, Out out) const
          noexcept(noexcept(*++out = c)) {
        *++out = c;
        return out;
    }
};
struct write_repeated_char {
    std::size_t count;
    template <class CharT, class Out>
    constexpr Out operator()(CharT c, Out out) const
          noexcept(noexcept(*++out = c)) {
        std::size_t n = count;
        while (n--)
            *++out = c;
        return out;
    }
};
struct write_string {
    template <class CharT, class Traits, class Out>
    constexpr Out operator()(std::basic_string_view<CharT, Traits> str,
                             Out out) const
          noexcept(noexcept(*++out = std::declval<CharT>())) {
        for (CharT c : str)
            *++out = c;
        return out;
    }
};

struct get_length {
    template <class CharT>
    constexpr std::size_t operator()(CharT) const noexcept {
        return 1;
    }
    template <class CharT>
    constexpr std::size_t operator()(basic_string_view<CharT> s) const
          noexcept {
        return s.size();
    }
};

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
        return std::visit(
              [&fc](auto w) {
                  if constexpr (std::is_same_v<decltype(w), std::size_t>) {
                      return w;
                  } else {
                      return visit_format_arg(
                            [](auto arg) -> std::size_t {
                                if constexpr (std::is_integral_v<decltype(
                                                    arg)>) {
                                    return arg;
                                } else {
                                    throw format_error("width is not integral");
                                }
                            },
                            fc.arg(w._id));
                  }
              },
              format_spec.width.i);
    }
    template <typename Out>
    static constexpr Out write_left_padding(
          std::size_t content_len,
          std::size_t width,
          Out out,
          std_format_spec_types::alignment_t align,
          CharT fill) {
        if (content_len >= width)
            return out;
        const auto n = width - content_len;
        const auto count = [&]() -> std::size_t {
            using A = std_format_spec_types::alignment_t;
            switch (align) {
                case A::center:
                    return n / 2;
                case A::left:
                    return 0;
                case A::right:
                    return n;
                case A::after_sign:
                    return n - 1;
                case A::defaulted:
                    LRSTD_UNREACHABLE();
            }
        }();
        return write_repeated_char{count}(fill, out);
    }
    template <typename Out>
    static constexpr Out write_right_padding(
          std::size_t content_len,
          std::size_t width,
          Out out,
          std_format_spec_types::alignment_t align,
          CharT fill) {
        if (content_len >= width)
            return out;
        const auto n = width - content_len;
        const auto count = [&]() -> std::size_t {
            using A = std_format_spec_types::alignment_t;
            switch (align) {
                case A::center:
                    return n - n / 2;
                case A::left:
                    return n;
                case A::right:
                    return 0;
                case A::after_sign:
                    return 0;
                case A::defaulted:
                    LRSTD_UNREACHABLE();
            }
        }();
        return write_repeated_char{count}(fill, out);
    }

    template <class Out, class FormatFunc, class Writer = write_string>
    constexpr Out do_format(basic_format_context<Out, CharT>& fc,
                            FormatFunc&& f,
                            const Writer& writer = {}) {
        const auto width = get_width(fc);
        const auto type = is_defaulted(format_spec.type) ? f.default_type()
                                                         : format_spec.type;
        const auto align = is_defaulted(format_spec.align)
                                 ? f.default_alignment(type)
                                 : format_spec.align;
        const auto formatted = f(type, format_spec.alternate);
        const auto flength = get_length{}(formatted);
        fc.advance_to(write_left_padding(flength, width, fc.out(), align,
                                         format_spec.fill));
        fc.advance_to(writer(formatted, fc.out()));
        fc.advance_to(write_right_padding(flength, width, fc.out(), align,
                                          format_spec.fill));
        return fc.out();
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

template <class Int>
struct format_int {
    typename format_int_storage_type<Int>::type i;
    std::array<char, sizeof(Int) * 8 + 2> buf;

    constexpr std::string_view operator()(std_format_spec_types::type_t type,
                                          bool write_prefix) {
        using Traits = std::char_traits<char>;
        auto to_chars = [this, write_prefix](
                              int base,
                              std::string_view prefix) -> std::string_view {
            auto start = std::begin(buf);
            if (write_prefix) {
                Traits::copy(start, prefix.data(), prefix.size());
                std::advance(start, prefix.size());
            }
            const auto result = std::to_chars(start, std::end(buf), i, base);
            assert(result.ec == std::errc());
            return std::string_view(std::begin(buf),
                                    std::distance(std::begin(buf), result.ptr));
        };
        using T = std_format_spec_types::type_t;
        switch (type) {
            case T::x:
                return to_chars(16, "0x");
            case T::X: {
                auto result = to_chars(16, "0X");
                convert_to_upper();
                return result;
            }
            case T::b:
                return to_chars(2, "0b");
            case T::B:
                return to_chars(2, "0B");

            case T::d:
                return to_chars(10, "");
            case T::o:
                return to_chars(8, "0");
            case std_format_spec_types::type_t::c:
                if (i < std::numeric_limits<char>::min() ||
                    std::numeric_limits<char>::max() < i)
                    throw format_error(
                          "integer is not in representable range of char");
                Traits::assign(buf[0], static_cast<char>(i));
                return std::string_view(buf.data(), 1);

            case T::n:
                throw "TODO";

            case T::s:
                if constexpr (std::is_same_v<Int, bool>) {
                    const std::string_view s =
                          static_cast<bool>(i) ? "true" : "false";
                    Traits::copy(std::begin(buf), s.data(), s.size());
                    return std::string_view(std::begin(buf), s.size());
                }
                [[fallthrough]];
            case T::a:
            case T::A:
            case T::e:
            case T::E:
            case T::f:
            case T::F:
            case T::g:
            case T::G:
            case T::p:
                throw format_error("invalid formatting type for integer");

            case T::defaulted:
                break;
        }
        LRSTD_UNREACHABLE();
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
        if constexpr (is_char_or_wchar_v<Int> || std::is_same_v<Int, bool>) {
            return is_integer_type(type) ? A::right : A::left;
        } else {
            return A::right;
        }
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
struct int_formatter : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    typename basic_format_context<Out, CharT>::iterator format(
          Int i,
          basic_format_context<Out, CharT>& fc) {
        return base::do_format(fc, format_int<Int>{i});
    }
};

template <class CharT>
struct formatter_impl<int, CharT, true> : public int_formatter<int, CharT> {};

template <class CharT>
struct formatter_impl<bool, CharT, true> : public int_formatter<bool, CharT> {};

template <class CharT>
struct formatter_impl<CharT, CharT, true>
    : public int_formatter<CharT, CharT> {};
template <>
struct formatter_impl<char, wchar_t, true>
    : public int_formatter<char, wchar_t> {};

//////////////////////////////
// string formatters

template <class CharT, class Traits>
struct get_str {
    std::basic_string_view<CharT, Traits> s;
    constexpr std::basic_string_view<CharT, Traits> operator()(
          std_format_spec_types::type_t type,
          bool) const {
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
};
template <class CharT>
struct formatter_impl<CharT*, CharT, true> : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    typename basic_format_context<Out, CharT>::iterator format(
          CharT* s,
          basic_format_context<Out, CharT>& fc) {
        using Traits = std::char_traits<CharT>;
        return base::do_format(fc, get_str<CharT, Traits>{s});
    }
};
template <class CharT>
struct formatter_impl<const CharT*, CharT, true>
    : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    typename basic_format_context<Out, CharT>::iterator format(
          const CharT* s,
          basic_format_context<Out, CharT>& fc) {
        using Traits = std::char_traits<CharT>;
        return base::do_format(fc, get_str<CharT, Traits>{s});
    }
};
template <class CharT, std::size_t N>
struct formatter_impl<const CharT[N], CharT, true>
    : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    typename basic_format_context<Out, CharT>::iterator format(
          const CharT* s,
          basic_format_context<Out, CharT>& fc) {
        using Traits = std::char_traits<CharT>;
        return base::do_format(fc, get_str<CharT, Traits>{s});
    }
};
template <class CharT, class Traits, class Alloc>
struct formatter_impl<std::basic_string<CharT, Traits, Alloc>, CharT, true>
    : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    typename basic_format_context<Out, CharT>::iterator format(
          std::basic_string<CharT, Traits> s,
          basic_format_context<Out, CharT>& fc) {
        return base::do_format(fc, get_str<CharT, Traits>{s});
    }
};
template <class CharT, class Traits>
struct formatter_impl<std::basic_string_view<CharT, Traits>, CharT, true>
    : public std_format_parser<CharT> {
    using base = std_format_parser<CharT>;
    template <typename Out>
    typename basic_format_context<Out, CharT>::iterator format(
          std::basic_string_view<CharT, Traits> s,
          basic_format_context<Out, CharT>& fc) {
        return base::do_format(fc, get_str<CharT, Traits>{s});
    }
};

}  // namespace detail

template <typename T, typename Char = char>
struct formatter : public detail::formatter_impl<T, Char> {};

namespace detail {

namespace parse_fmt_str {
using end = std::monostate;
struct text {};
struct escaped_lbrace {};
struct escaped_rbrace {};

template <class CharT>
struct replacement_field {
    arg_id_t arg_id;
    basic_string_view<CharT> format_spec;
};

using namespace parse_utils;

template <class CharT>
using result = std::variant<end,
                            text,
                            escaped_lbrace,
                            escaped_rbrace,
                            replacement_field<CharT>,
                            error>;

template <class CharT>
constexpr std::variant<std::size_t, std::nullopt_t, error> parse_arg_id(
      basic_string_view<CharT>& fmt) {
    if (consume(fmt, static_cast<CharT>('0')))
        return 0;
    return parse_integer(fmt);
}

template <class CharT>
struct get_replacement_field {
    constexpr std::optional<replacement_field<CharT>> operator()(
          std::size_t id_) const noexcept {
        return replacement_field<CharT>{arg_id_t{id_}, {}};
    }
    constexpr std::optional<replacement_field<CharT>> operator()(
          std::nullopt_t) const noexcept {
        return replacement_field<CharT>{arg_id_t::auto_id(), {}};
    }
    constexpr std::optional<replacement_field<CharT>> operator()(error) const
          noexcept {
        return std::nullopt;
    }
};

template <class CharT>
constexpr std::optional<replacement_field<CharT>> parse_replacement_field(
      basic_string_view<CharT>& fmt) {
    auto field = std::visit(get_replacement_field<CharT>{}, parse_arg_id(fmt));
    if (!field)
        return std::nullopt;
    if (consume(fmt, static_cast<CharT>(':'))) {
        auto format_spec_end = find_balanced_delimiter_end(
              fmt, static_cast<CharT>('{'), static_cast<CharT>('}'));
        field->format_spec = basic_string_view<CharT>(
              fmt.begin(), std::distance(fmt.begin(), format_spec_end));
        advance_to(fmt, format_spec_end);
    }
    return field;
}

template <class CharT>
struct double_lbrace;
template <>
struct double_lbrace<char> {
    static constexpr std::string_view value = "{{";
};
template <>
struct double_lbrace<wchar_t> {
    static constexpr std::wstring_view value = L"{{";
};

template <class CharT>
struct double_rbrace;
template <>
struct double_rbrace<char> {
    static constexpr std::string_view value = "}}";
};
template <>
struct double_rbrace<wchar_t> {
    static constexpr std::wstring_view value = L"}}";
};

template <class CharT>
constexpr result<CharT> parse_next(basic_string_view<CharT>& fmt) {
    if (fmt.empty())
        return end{};

    if (consume(fmt, double_lbrace<CharT>::value))
        return escaped_lbrace{};
    if (consume(fmt, double_rbrace<CharT>::value))
        return escaped_rbrace{};

    const auto next_lbrace_it = find(fmt, static_cast<CharT>('{'));
    if (next_lbrace_it != fmt.begin()) {
        advance_to(fmt, next_lbrace_it);
        return text{};
    }

    // beginning is '{'
    fmt.remove_prefix(1);
    const auto result = parse_replacement_field(fmt);
    if (!result || !consume(fmt, static_cast<CharT>('}')))
        return error{};
    return *result;
}

}  // namespace parse_fmt_str

namespace fmt_out {
template <class CharT, class Out>
Out text_out(basic_string_view<CharT> text, Out out) {
    for (auto& c : text)
        *++out = c;
    return out;
}

template <class CharT, class Out>
Out chars_out(CharT c, std::size_t count, Out out) {
    while (count--)
        *++out = c;
    return out;
}

template <typename T, typename Ret = void>
struct throw_uninitialized_format_arg {
    Ret operator()(const T&) const {
        throw format_error("uninitialized format argument");
    }
};

template <class Context, class CharT>
void arg_out(Context& fc,
             basic_format_parse_context<CharT>& pc,
             const basic_format_arg<Context>& arg) {
    visit_format_arg(
          overloaded{
                throw_uninitialized_format_arg<std::monostate>{},
                [&](const typename basic_format_arg<Context>::handle& handle) {
                    handle.format(pc, fc);
                },
                [&](const auto& val) {
                    using T = std::remove_cv_t<
                          std::remove_reference_t<decltype(val)>>;
                    typename Context::template formatter_type<T> f;
                    pc.advance_to(f.parse(pc));
                    fc.advance_to(f.format(val, fc));
                }},
          arg);
}
}  // namespace fmt_out

template <typename T, typename Ret = void>
struct unreachable {
    Ret operator()(const T&) const { LRSTD_UNREACHABLE(); }
};

template <class CharT, class Out>
Out vformat_to_impl(Out out,
                    basic_string_view<CharT> fmt,
                    format_args_t<Out, CharT>& args) {
    basic_format_context<Out, CharT> context(args, out);
    basic_format_parse_context<CharT> parse_context(fmt, context.args_size());

    namespace p = parse_fmt_str;
    while (true) {
        auto old_begin = fmt.begin();
        p::result<CharT> r = p::parse_next(fmt);
        if (std::holds_alternative<p::end>(r))
            return out;
        if (std::holds_alternative<p::error>(r))
            throw format_error("bad format string");
        std::visit(
              overloaded{
                    unreachable<p::end>{}, unreachable<p::error>{},
                    [&](p::text) {
                        fmt_out::text_out(
                              basic_string_view<CharT>{
                                    old_begin,
                                    static_cast<std::size_t>(std::distance(
                                          old_begin, fmt.begin()))},
                              out);
                    },
                    [&](p::escaped_lbrace) {
                        const CharT c = static_cast<CharT>('{');
                        fmt_out::text_out(basic_string_view<CharT>{&c, 1}, out);
                    },
                    [&](p::escaped_rbrace) {
                        const CharT c = static_cast<CharT>('}');
                        fmt_out::text_out(basic_string_view<CharT>{&c, 1}, out);
                    },
                    [&](p::replacement_field<CharT> field) {
                        parse_context._begin = field.format_spec.begin();
                        parse_context._end = field.format_spec.end();
                        fmt_out::arg_out(
                              context, parse_context,
                              context.arg(field.arg_id.is_auto()
                                                ? parse_context.next_arg_id()
                                                : (parse_context.check_arg_id(
                                                         field.arg_id._id),
                                                   field.arg_id._id)));
                    }},
              r);
    }
}  // namespace detail

template <class CharT>
std::basic_string<CharT> vformat_impl(
      basic_string_view<CharT> fmt,
      format_args_t<std::back_insert_iterator<std::basic_string<CharT>>, CharT>
            args) {
    std::basic_string<CharT> ret;
    vformat_to_impl(std::back_inserter(ret), fmt, args);
    return ret;
}

}  // namespace detail

template <class Out>
Out vformat_to(Out out, std::string_view fmt, format_args_t<Out, char> args) {
    return detail::vformat_to_impl(out, fmt, args);
}

template <class Out>
Out vformat_to(Out out,
               std::wstring_view fmt,
               format_args_t<Out, wchar_t> args) {
    return detail::vformat_to_impl(out, fmt, args);
}

template <class Out, class... Args>
Out format_to(Out out, std::string_view fmt, const Args&... args) {
    using Context = basic_format_context<Out, std::string_view::value_type>;
    return vformat_to(out, fmt, {make_format_args<Context>(args...)});
}

template <class Out, class... Args>
Out format_to(Out out, std::wstring_view fmt, const Args&... args) {
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

}  // namespace lrstd

#endif
