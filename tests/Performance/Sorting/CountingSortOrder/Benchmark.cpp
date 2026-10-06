#include "../../../../src/Sorting/CountingSortOrder/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random() % n);
    std::vector<int> expected(n);
    std::iota(expected.begin(), expected.end(), 0);
    std::stable_sort(expected.begin(), expected.end(), [&](int i, int j) { return a[i] < a[j]; });
    return measure(input, "stable counting sort; uniform keys / duplicates",
                   [&]() -> std::uint64_t {
                       auto b = countingSortOrder(a);
                       benchmarkCheck(b == expected, "stable permutation against std::stable_sort");
                       std::uint64_t checksum = 0;
                       for (int i : b) checksum = benchmarkMix(checksum, i);
                       return checksum;
                   });
}
