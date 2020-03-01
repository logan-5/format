// #define PROFILING

#ifdef PROFILING
#define CATCH_CONFIG_RUNNER
#else
#define CATCH_CONFIG_MAIN
#endif

#include <catch.hpp>

// test double includes

#include "format.hpp"

#include "format.hpp"

#include "converter.hpp"

#include "converter.hpp"

#include "user_defined.hpp"

#include "user_defined.hpp"

#include <iostream>

#ifdef PROFILING
template <class T>
void doNotOptimizeAway(T&& datum) {
    asm volatile("" : "+r"(datum));
}

int main() {
	format(("{0}-{{"), 8);
    for (;;) {
        std::array<char, 24> data{};
        doNotOptimizeAway(lrstd::format_to(data.data(), "{}", 42) -
                          data.data());
        //		std::cout << lrstd::format_to(data.data(), "{}", 42) -
        //		data.data();
        //		doNotOptimizeAway(lrstd::format(data.data(), "{}",
        // 42).size());
    }
}
#endif
