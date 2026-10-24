#include "../../../../../src/extra/DataStructures/BitRMQ/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<int> a(in.n);
    for (int i = 0; i < in.n; ++i) {
        a[i] = in.shape ? in.n - i : i;
    }
    std::vector<std::pair<int, int>> queries;
    for (int i = 0; i < in.n; ++i) {
        int l = int(rng() % in.n), length = 1;
        switch (i % 6) {
            case 0: length = 1; break;
            case 1: length = 64 - l % 64; break;
            case 2: length = 65 - l % 64; break;
            case 3: length = 63 + (i / 6) % 3; break;
            case 4: length = 1 + int(rng() % (in.n - l)); break;
            case 5: length = in.n - l; break;
        }
        queries.push_back({l, std::min(in.n, l + length)});
    }
    return measure(in, "increasing / decreasing bit RMQ with mixed block ranges", [&]() -> std::uint64_t {
        BitRMQ<int> t(a);
        std::uint64_t sum = 0;
        for (auto [l, r] : queries) {
            int value = t(l, r);
            benchmarkCheck(value == a[in.shape ? r - 1 : l], "monotone interval minimum");
            sum = benchmarkMix(sum, value);
        }
        return sum;
    });
}
