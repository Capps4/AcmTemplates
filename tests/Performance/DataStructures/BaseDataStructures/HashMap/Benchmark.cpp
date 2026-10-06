#include "../../../../../src/DataStructures/BaseDataStructures/HashMap/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<long long> a(n);
    for (int i = 0; i < n; ++i)
        a[i] = input.shape ? i % 16 : static_cast<long long>(random());
    return measure(input, "fixed-capacity hash insert and lookup; random / duplicates",
                   [&]() -> std::uint64_t {
                       _hashmap::Impl<long long, 262147, 300000> h;
                       for (auto x : a)
                           ++h[x];
                       std::uint64_t sum = 0;
                       for (auto x : a)
                           sum += h(x);
                       return sum;
                   });
}
