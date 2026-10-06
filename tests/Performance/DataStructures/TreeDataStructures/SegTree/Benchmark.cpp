#include "../../../../../src/DataStructures/TreeDataStructures/SegTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
#include "../../../../Correctness/DataStructures/TreeDataStructures/SegTree/TestTypes.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<long long> a(n, 1);
    return measure(input, "lazy range updates and sums; short / full ranges",
                   [&]() -> std::uint64_t {
                       auto d = SegTree<SumInfo, AddTag>(a);
                       std::uint64_t sum = 0;
                       for (int i = 0; i < n; ++i) {
                           int l = input.shape ? 0 : i, r = input.shape ? n : std::min(n, i + 100);
                           d.modify(l, r, AddTag{1});
                           sum += d.query(l, r).val;
                       }
                       return sum;
                   });
}
