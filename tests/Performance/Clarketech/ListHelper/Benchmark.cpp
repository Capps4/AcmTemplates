#include "../../../../src/Clarketech/ListHelper/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<int> a(n);
    for (auto &x : a)
        x = input.shape ? 7 : int(random());
    return measure(input, "sort/unique/map pipeline; random / duplicates", [&]() -> std::uint64_t {
        auto b = a | sorted() | unique() | map([](int x) {
                     return static_cast<long long>(x);
                 });
        return std::accumulate(b.begin(), b.end(), std::uint64_t(0));
    });
}
