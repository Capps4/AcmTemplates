#pragma once
#include <cstdlib>
#include <cstdint>
#include <iostream>
#include <random>

inline std::uint64_t testSeed = [] {
    const char* seed = std::getenv("TEST_SEED");
    return seed ? std::strtoull(seed, nullptr, 10) : 20261001ULL;
}();
inline std::mt19937_64 testRng(testSeed);
inline int randomInt(int l, int r) {
    return std::uniform_int_distribution<int>(l, r)(testRng);
}
inline void check(bool passed, const char* expression, int line, const char* file) {
    if (!passed) {
        std::cerr << "Failed at " << file << ":" << line << ": " << expression << "; TEST_SEED=" << testSeed << '\n';
        std::exit(1);
    }
}
#define CHECK(...) check(bool((__VA_ARGS__)), #__VA_ARGS__, __LINE__, __FILE__)
