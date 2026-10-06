#include "../../../../../src/DataStructures/TreeDataStructures/FenwickTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 0 : int(random() % 10);
    return measure(input, "Fenwick updates and prefixes", [&]() -> std::uint64_t {
        Fenwick<long long> d(n);
        for (int i = 0; i < n; ++i)
            d.modify(i, a[i]);
        std::uint64_t sum = 0;
        for (int i = 0; i < n; ++i)
            sum += d.query(i);
        return sum;
    });
}
