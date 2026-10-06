#include "../../../../src/Sorting/MoSort/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    Query::rangeScale(n);
    std::vector<Query> a;
    for (int i = 0; i < n; ++i) {
        int l = input.shape ? i : int(random() % n), r = input.shape ? n : int(random() % n);
        if (l > r)
            std::swap(l, r);
        a.emplace_back(l, r, i);
    }
    return measure(input, "Mo order and pointer movement; random / suffix ranges",
                   [&]() -> std::uint64_t {
                       auto q = a;
                       std::sort(q.begin(), q.end());
                       std::uint64_t sum = 0;
                       int l = 0, r = 0;
                       for (auto x : q) {
                           sum += std::abs(l - x.l) + std::abs(r - x.r);
                           l = x.l;
                           r = x.r;
                       }
                       return sum;
                   });
}
