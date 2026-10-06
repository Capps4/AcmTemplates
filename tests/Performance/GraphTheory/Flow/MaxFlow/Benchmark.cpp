#include "../../../../../src/GraphTheory/Flow/MaxFlow/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    return measure(input, "deep chain / wide parallel augmenting paths",
                   [&]() -> std::uint64_t {
                       Flow<int> flow(n + 2);
                       if (input.shape) {
                           for (int i = 1; i <= n; ++i) {
                               flow.add(0, i, 1);
                               flow.add(i, n + 1, 1);
                           }
                       } else {
                           for (int i = 0; i <= n; ++i) flow.add(i, i + 1, 3);
                       }
                       auto value = flow.work(0, n + 1);
                       benchmarkCheck(value == (input.shape ? n : 3), "known min-cut capacity");
                       return value;
                   });
}
