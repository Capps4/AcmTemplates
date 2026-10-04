#include <bits/stdc++.h>
#include "../BenchmarkSupport.hpp"
namespace Legacy {
#include "../ModuloInteger/Original.hpp"
#include "Original.hpp"
}
#include "Final.hpp"
int main() {
    const int n = 300000;
    compare("factorial-build", [&] {
        auto c = Legacy::Comb<Legacy::Z>::shared(); c.init(n);
        return std::uint64_t(c.jc(n).val()) + c.ijc(n).val();
    }, [&] {
        Comb<Z> c(n); return std::uint64_t(c.jc(n).val()) + c.ijc(n).val();
    });
    auto old = Legacy::Comb<Legacy::Z>::shared(); old.init(n);
    Comb<Z> current(n);
    compare("combination-queries", [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 500000; ++i) sum += old.C(n, i % (n + 1)).val();
        return sum;
    }, [&] {
        std::uint64_t sum = 0;
        for (int i = 0; i < 500000; ++i) sum += current.C(n, i % (n + 1)).val();
        return sum;
    });
}
