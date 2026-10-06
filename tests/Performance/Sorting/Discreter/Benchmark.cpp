#include "../../../../src/Sorting/Discreter/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n), b;
    for (auto &x : a)
        x = input.shape ? 7 : int(random());
    b = a;
    std::sort(b.begin(), b.end());
    b.erase(std::unique(b.begin(), b.end()), b.end());
    return measure(input, "rank mapping against prebuilt sorted basis", [&]() -> std::uint64_t {
        auto r = a | discreteFrom(b);
        return std::accumulate(r.begin(), r.end(), std::uint64_t(0));
    });
}
