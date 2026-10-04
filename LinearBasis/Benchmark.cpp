#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main() {
    std::mt19937_64 rng(149);
    std::vector<std::uint64_t> input(200000);
    for (auto& value : input) value = rng() & ((1ULL << 60) - 1);
    compare("insert-200K", [&] {
        Legacy::LinearBasis<std::uint64_t> basis;
        for (auto value : input) basis.insert(value);
        return basis.getMax() ^ std::uint64_t(basis.rank);
    }, [&] {
        LinearBasis<std::uint64_t> basis;
        for (auto value : input) basis.insert(value);
        return basis.getMax() ^ std::uint64_t(basis.rank);
    });
    Legacy::LinearBasis<std::uint64_t> old;
    LinearBasis<std::uint64_t> current;
    for (int i = 0; i < 30; ++i) { old.insert(input[i]); current.insert(input[i]); }
    compare("max-min-check-200K", [&] {
        std::uint64_t sum = 0;
        for (auto value : input) sum += old.getMax(value) ^ old.getMin(value) ^ std::uint64_t(old.check(value));
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (auto value : input) sum += current.getMax(value) ^ current.getMin(value) ^ std::uint64_t(current.check(value));
        return sum;
    });
    compare("kth-200K", [&] {
        std::uint64_t sum = 0;
        for (auto value : input) sum ^= old.findByOrder(value & ((1u << 20) - 1));
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (auto value : input) sum ^= current.findByOrder(value & ((1u << 20) - 1));
        return sum;
    });
}
