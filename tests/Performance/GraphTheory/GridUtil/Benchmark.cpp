#include "../../../../src/GraphTheory/GridUtil/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    (void)random;
    GridUtil g(input.shape ? 1 : 256, input.shape ? n : 256);
    return measure(input, "allocation-free neighbor traversal; interior grid / one-row boundary",
                   [&]() -> std::uint64_t {
                       std::uint64_t sum = 0;
                       for (int i = 0; i < n; ++i) {
                           int x = input.shape ? 0 : i % 256, y = input.shape ? i : (i / 256) % 256;
                           g.forEachNeighbor(x, y, [&](int a, int b) {
                               sum += std::uint64_t(a) + b + 1;
                           });
                       }
                       return sum;
                   });
}
