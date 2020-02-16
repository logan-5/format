#pragma once

#include <forward_list>
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
    constexpr char operator()(char c) const noexcept { return c; }
};

// hangs on to every result it produces so that references and views remain
// valid. so keep these in smallish scopes, and careful with loops
template <>
struct str_fn<wchar_t> {
    std::forward_list<std::wstring> bufs;

    const wchar_t* operator()(const char* c) {
        auto& buf = bufs.emplace_front(c, c + std::strlen(c));
        return buf.c_str();
    }
    const std::wstring& operator()(const std::string& s) {
        auto& buf = bufs.emplace_front(s.begin(), s.end());
        return buf;
    }
    std::wstring_view operator()(std::string_view s) {
        auto& buf = bufs.emplace_front(s.begin(), s.end());
        return buf;
    }
    constexpr wchar_t operator()(char c) const noexcept { return c; }
};

template <class Char>
struct SV {
    template <std::size_t N>
    std::basic_string_view<Char> operator()(
          const std::array<Char, N>& arr) const {
        return {arr.data()};
    }
    template <class C>
    std::basic_string_view<Char> operator()(const C& c) const {
        return {std::data(c), std::size(c)};
    }
};
