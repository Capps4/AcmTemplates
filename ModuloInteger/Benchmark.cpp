#include <bits/stdc++.h>
#include "Final.hpp"
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "Original.hpp"
}
template<class M>
std::uint64_t work(const std::vector<int>& values) {
    M sum = 1;
    for (int x : values) { sum += M(x); sum *= M(7); sum -= M(3); }
    return sum.val();
}
int main() {
    std::mt19937 rng(20261001);
    std::vector<int> values(1000000);
    for (int& x : values) x = rng() % 100000;
    compare("fixed-int-arithmetic-1M", [&] { return work<Legacy::Z>(values); }, [&] { return work<Z>(values); });
    using Before = Legacy::ModuloInteger<int, 0>;
    using After = ModuloInteger<int, 0>;
    Before::setMod(1000000007); After::setMod(1000000007);
    compare("dynamic-int-arithmetic-1M", [&] { return work<Before>(values); }, [&] { return work<After>(values); });
    // Old long-long multiplication is only compared where all signed intermediates fit.
    using OldWide = Legacy::ModuloInteger<long long, 0>;
    using NewWide = ModuloInteger<long long, 0>;
    OldWide::setMod(1000003); NewWide::setMod(1000003);
    compare("safe-dynamic-long-long-1M", [&] { return work<OldWide>(values); }, [&] { return work<NewWide>(values); });
}
