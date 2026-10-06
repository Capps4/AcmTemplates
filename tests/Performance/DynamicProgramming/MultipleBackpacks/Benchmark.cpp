#include "../../../../src/DynamicProgramming/MultipleBackpacks/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    VecGood goods;
    for (int i = 1; i <= 100; ++i)
        goods.push_back({i, input.shape ? 1 : i, 1000});
    (void)random;
    return measure(input, "100 bounded item types: varied / equal weights", [&]() -> std::uint64_t {
        auto a = multiBag(goods, n);
        return a.back();
    });
}
