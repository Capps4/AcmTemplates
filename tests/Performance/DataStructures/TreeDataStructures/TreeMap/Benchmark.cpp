#include "../../../../../src/DataStructures/TreeDataStructures/TreeMap/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random());
    return measure(input, "ordered map insert, lookup and rank; random / duplicates",
                   [&]() -> std::uint64_t {
                       TreeMap<int, int> d(n);
                       for (auto x : a)
                           d.insertOrAssign(x, 1);
                       std::uint64_t sum = 0;
                       for (auto x : a)
                           sum += d.rankOf(x) + d(x).value();
                       return sum;
                   });
}
