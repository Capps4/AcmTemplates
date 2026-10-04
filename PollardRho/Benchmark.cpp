#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "../ModuloInteger/Original.hpp"
#include "../MillerRabin/Original.hpp"
#include "Original.hpp"
}
#include "Final.hpp"
// Compare only well-defined small legacy arithmetic; seed is unavailable in legacy.
int main() {
    std::mt19937 rng(17);
    std::vector<int> mixed(5000), composites(2000);
    for (auto& x : mixed) x = 2 + rng()%999998;
    for (auto& x : composites) x = (101 + rng()%900) * (101 + rng()%900);
    auto checksum = [](auto& factorizer, const auto& values) {
        std::uint64_t result=0;
        for (auto n : values) for (auto [p,e] : factorizer.primeFactorize(n)) result+=std::uint64_t(p)*e;
        return result;
    };
    Legacy::PollardRho<int> old;
    compare("mixed-small", [&] { return checksum(old,mixed); },
            [&] { PollardRho<int> current(7); return checksum(current,mixed); });
    compare("composites-small", [&] { return checksum(old,composites); },
            [&] { PollardRho<int> current(7); return checksum(current,composites); });
}
