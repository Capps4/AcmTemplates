#include "../../../../../src/extra/DataStructures/DeletableHeap/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random());
    std::vector<int> expected;
    for (int i = 1; i < n; i += 2) {
        expected.push_back(a[i]);
    }
    std::sort(expected.begin(), expected.end(), std::greater<int>{});
    return measure(input, "heap push, delayed deletion, drain; random / duplicates",
                   [&]() -> std::uint64_t {
                       DeletableHeap<int> d;
                       for (auto x : a)
                           d.push(x);
                       for (int i = 0; i < n; i += 2)
                           d.erase(a[i]);
                       std::uint64_t sum = 0;
                       std::size_t index = 0;
                       while (not d.empty()) {
                           int value = d.top();
                           benchmarkCheck(index < expected.size() and value == expected[index],
                                          "sorted survivors in descending order");
                           sum = benchmarkMix(sum, std::uint64_t(value));
                           ++index;
                           d.pop();
                       }
                       benchmarkCheck(index == expected.size(), "survivor count");
                       return sum;
                   });
}
