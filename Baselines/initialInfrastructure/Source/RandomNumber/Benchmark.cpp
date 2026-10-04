#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
#include "Final.hpp"
int main() {
    compare("mt19937-64-1M", [] {
        Legacy::rng.seed(17);
        std::uint64_t sum = 0;
        for (int i = 0; i < 1000000; ++i) sum ^= Legacy::rng();
        return sum;
    }, [] {
        rng.seed(17);
        std::uint64_t sum = 0;
        for (int i = 0; i < 1000000; ++i) sum ^= rng();
        return sum;
    });
}
