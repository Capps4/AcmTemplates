#include "../../../../../src/DataStructures/BaseDataStructures/RMQ/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random());
    std::vector<std::pair<int, int>> q;
    for (int i = 0; i < n; ++i) {
        int l = int(random() % n), r = int(random() % n);
        if (l > r)
            std::swap(l, r);
        q.emplace_back(l, r + 1);
    }
    return measure(input, "RMQ build and range queries", [&]() -> std::uint64_t {
        RMQ<int> d(a);
        std::uint64_t sum = 0;
        for (auto [l, r] : q)
            sum += std::uint64_t(d(l, r));
        return sum;
    });
}
