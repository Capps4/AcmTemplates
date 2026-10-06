#include "../../../../../src/DataStructures/BaseDataStructures/DeletableHeap/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random());
    return measure(input, "heap push, delayed deletion, drain; random / duplicates",
                   [&]() -> std::uint64_t {
                       DeletableHeap<int> d;
                       for (auto x : a)
                           d.push(x);
                       for (int i = 0; i < n; i += 2)
                           d.erase(a[i]);
                       std::uint64_t sum = 0;
                       while (!d.empty()) {
                           sum += std::uint64_t(d.top());
                           d.pop();
                       }
                       return sum;
                   });
}
