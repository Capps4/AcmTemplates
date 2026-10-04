#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
#include "../ModuloInteger/Final.hpp"
namespace Legacy {
// Rename the old helper to avoid ADL finding the new global gauss for global Z.
#define gauss legacyGauss
#include "Original.hpp"
#undef gauss
}
#include "Final.hpp"
template<class T, class Result> std::uint64_t checksum(const Result& inverse) {
    if (inverse.status != "OK") std::exit(1);
    std::uint64_t sum = 0;
    for (std::size_t x = 0; x < inverse.inv.size(); ++x) for (std::size_t y = 0; y < inverse.inv[x].size(); ++y) {
        std::uint64_t value;
        if constexpr (std::is_floating_point_v<T>) value = std::uint64_t(std::llround(inverse.inv[x][y] * 1e6));
        else value = inverse.inv[x][y].val();
        sum += value * (x + 1) * (y + 1);
    }
    return sum;
}
template<class T> void scenario(int n, const char* type) {
    std::mt19937 rng(179);
    std::vector<std::vector<T>> matrix(n, std::vector<T>(n));
    for (int x = 0; x < n; ++x) for (int y = 0; y < n; ++y)
        matrix[x][y] = x == y ? n * 10 : int(rng() % 9) - 4;
    auto name = std::string(type) + "-inverse-" + std::to_string(n);
    compare(name.c_str(), [&] { return checksum<T>(Legacy::MatrixUtil<T>(matrix)); },
            [&] { return checksum<T>(MatrixUtil<T>(matrix)); });
}
int main() { for (int n : {40, 100}) { scenario<double>(n, "floating"); scenario<Z>(n, "modular"); } }
