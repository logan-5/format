#include <list>
#include <string>

template <class Char>
struct str_fn;

// convert between regular and wide string literals for template test cases

// these do some sketchy stuff indeed, just need something that "works"

template <>
struct str_fn<char> {
    constexpr const char* operator()(const char* c) const noexcept { return c; }
    const std::string& operator()(const std::string& s) const noexcept {
        return s;
    }
    constexpr std::string_view operator()(std::string_view s) const noexcept {
        return s;
    }
};
template <>
struct str_fn<wchar_t> {
    std::list<std::wstring> bufs;

    const wchar_t* operator()(const char* c) {
        auto& buf = bufs.emplace_back(c, c + std::strlen(c));
        return buf.c_str();
    }
    const std::wstring& operator()(const std::string& s) {
        auto& buf = bufs.emplace_back(s.begin(), s.end());
        return buf;
    }
    std::wstring_view operator()(std::string_view s) {
        auto& buf = bufs.emplace_back(s.begin(), s.end());
        return buf;
    }
};
