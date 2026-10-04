#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
int main() {
    std::mt19937 rng(20261001);
    std::vector<std::pair<long long, long long>> values(200000);
    for (auto& [a, b] : values) { a = rng() % 1000000000; b = rng() % 1000000000; }
    compare("random-1e9", [&] {
        std::uint64_t sum = 0;
        for (auto [a, b] : values) { long long x, y; auto g = Legacy::exgcd(a, b, x, y); sum += std::uint64_t(g) + std::uint64_t(x) + std::uint64_t(y); }
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (auto [a, b] : values) { long long x, y; auto g = exgcd(a, b, x, y); sum += std::uint64_t(g) + std::uint64_t(x) + std::uint64_t(y); }
        return sum;
    });
}
