#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
std::uint64_t checksum(const std::vector<long long>& values) {
    std::uint64_t sum = 0;
    for (std::size_t i = 0; i < values.size(); ++i) sum += std::uint64_t(values[i]) * (i + 1);
    return sum;
}
int main() {
    std::mt19937 rng(139);
    VecGood goods(100);
    for (auto& item : goods) item = {int(rng() % 100 + 1), int(rng() % 100 + 1), int(rng() % 100 + 1)};
    compare("positive-items-capacity-20K", [&] { return checksum(Legacy::multiBag(goods, 20000)); },
            [&] { return checksum(multiBag(goods, 20000)); });
    goods.assign(100, {1, 200000, 1});
    compare("oversized-items-100x", [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 100; ++i) { sum += checksum(Legacy::multiBag(goods, 2000)); benchmarkConsume(sum); }
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 100; ++i) { sum += checksum(multiBag(goods, 2000)); benchmarkConsume(sum); }
        return sum;
    });
}
