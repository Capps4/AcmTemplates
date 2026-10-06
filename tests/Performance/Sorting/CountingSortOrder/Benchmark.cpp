#include "../../../../src/Sorting/CountingSortOrder/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random() % n);
    return measure(input, "stable counting sort; uniform keys / duplicates",
                   [&]() -> std::uint64_t {
                       auto b = countingSortOrder(a);
                       return std::accumulate(b.begin(), b.end(), std::uint64_t(0));
                   });
}
