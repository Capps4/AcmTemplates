#include "../../../../../src/extra/Math/FloorSum/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    std::mt19937_64 rng(input.seed);
    std::vector<std::array<std::uint64_t, 4>> queries(input.n);
    for (auto &q : queries) {
        q[0] = rng() & 0xffffffffULL;
        q[1] = input.shape ? 7540113804746346429ULL : rng() | 1;
        q[2] = input.shape ? 4660046610375530309ULL : rng();
        q[3] = rng();
    }
    using U = unsigned __int128;
    std::uint64_t expected = 0;
    for (auto [n, m, a, b] : queries) {
        U value = floorSum(n, m, a, b);
        U prefix = 0;
        auto count = std::min(n, std::uint64_t(64));
        for (std::uint64_t i = 0; i < count; ++i)
            prefix += (U(a) * i + b) / m;
        benchmarkCheck(floorSum(count, m, a, b) == prefix, "direct short-prefix sum");
        if (n != 0) {
            U last = U(a) * (n - 1) + b;
            auto rev = m - 1 - std::uint64_t(last % m);
            U mirror = floorSum(n, m, a, rev);
            // Opposite indices have numerators summing to (q + 1)*m - 1.
            benchmarkCheck(value + mirror == U(n) * (last / m), "reversed-index floor identity");
        } else {
            benchmarkCheck(value == 0, "empty sum");
        }
        expected = benchmarkMix(expected, std::uint64_t(value) ^ std::uint64_t(value >> 64));
    }
    return measure(input, "uint64 floor sums: random / consecutive Fibonacci divisors", [&] {
        std::uint64_t sum = 0;
        for (auto [n, m, a, b] : queries) {
            auto value = floorSum(n, m, a, b);
            sum = benchmarkMix(sum, std::uint64_t(value) ^ std::uint64_t(value >> 64));
        }
        benchmarkCheck(sum == expected, "validated floor sum checksum");
        return sum;
    });
}
