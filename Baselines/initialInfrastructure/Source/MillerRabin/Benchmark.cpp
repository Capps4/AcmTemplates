#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "../ModuloInteger/Original.hpp"
#include "Original.hpp"
}
#include "Final.hpp"
int main() {
    std::vector<int> values(200000);
    std::mt19937 rng(31);
    for (auto& x : values) x = rng() % 1000000;
    Legacy::MillerRabin<int> old32; Legacy::MillerRabin<long long> old64;
    MillerRabin<int> current32;
    compare("mixed-32bit", [&] { std::uint64_t sum=0; for(auto x:values) sum+=old32(x); return sum; },
            [&] { std::uint64_t sum=0; for(auto x:values) sum+=current32(x); return sum; });
    // Legacy 64-bit modular multiplication is well-defined only for these small values.
    compare("mixed-small-64bit", [&] { std::uint64_t sum=0; for(auto x:values) sum+=old64(x); return sum; },
            [&] { std::uint64_t sum=0; for(auto x:values) sum+=isPrime(x); return sum; });
}
