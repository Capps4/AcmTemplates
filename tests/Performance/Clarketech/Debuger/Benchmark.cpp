#include "../../../../src/Clarketech/Debuger/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    (void)random;
    std::free($);
    $ = nullptr;
    return measure(input,
                   "debug configuration allocation; disabled macro has no runtime side effects",
                   [&]() -> std::uint64_t {
                       std::uint64_t sum = 0;
                       for (int i = 0; i < n; ++i) {
                           char *p = strdup("color: false, space: false, precision: 6");
                           benchmarkSink = std::uint64_t(reinterpret_cast<std::uintptr_t>(p));
                           sum += std::strlen(p);
                           std::free(p);
                           debug(++sum);
                       }
                       return sum;
                   });
}
