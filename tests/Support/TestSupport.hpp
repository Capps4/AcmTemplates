#pragma once
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>

inline std::uint64_t testSeed = [] {
    const char* seed = std::getenv("TEST_SEED");
    return seed ? std::strtoull(seed, nullptr, 10) : 20261001ULL;
}();
inline std::mt19937_64 testRng(testSeed);
namespace test_context {
inline std::string id, input;
inline int step = -1;
template <class T>
void describe(const std::vector<T> &values) {
    std::ostringstream out;
    out << '[';
    for (std::size_t i = 0; i < values.size() and i < 64; ++i) {
        if (i) out << ',';
        out << values[i];
    }
    if (values.size() > 64) out << ",...;size=" << values.size();
    out << ']'; input = out.str();
}
inline void describe(std::string_view value) {
    std::ostringstream out;
    out << "bytes[";
    for (unsigned char c : value) out << unsigned(c) << ',';
    out << ']'; input = out.str();
}
}
inline int randomInt(int l, int r) {
    return std::uniform_int_distribution<int>(l, r)(testRng);
}
inline void check(bool passed, const char* expression, int line, const char* file) {
    if (!passed) {
        std::cerr << "CASE_FAIL " << test_context::id << ": " << expression
                  << "; " << file << ":" << line << "; TEST_SEED=" << testSeed;
        if (test_context::step >= 0)
            std::cerr << "; step=" << test_context::step;
        if (not test_context::input.empty())
            std::cerr << "; input=" << test_context::input;
        std::cerr << std::endl;
        std::exit(1);
    }
}
#define CHECK(...) ::check(bool((__VA_ARGS__)), #__VA_ARGS__, __LINE__, __FILE__)

template <class Actual, class Expected>
void checkEqual(const Actual &actual, const Expected &expected, const char *expression,
                int line, const char *file) {
    if (actual == expected)
        return;
    std::cerr << "expected=" << expected << "; actual=" << actual << '\n';
    check(false, expression, line, file);
}
#define CHECK_EQ(actual, expected) ::checkEqual((actual), (expected), #actual " == " #expected, __LINE__, __FILE__)
