#include "../../../../src/GraphTheory/RingTree/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> next(n);
    for (int i = 0; i < n; ++i)
        next[i] = input.shape ? 0 : (i + 1) % n;
    (void)random;
    return measure(input, "large cycle / many leaves into a self-cycle", [&]() -> std::uint64_t {
        RingTree a(next);
        std::uint64_t sum = 0;
        for (auto &r : a.rings)
            sum += r.size();
        for (auto &row : a.g)
            sum += row.size();
        return sum;
    });
}
