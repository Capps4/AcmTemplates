#include "../../../../../src/extra/Math/ExCrt/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    using i64 = long long;
    std::mt19937_64 random(in.seed);
    // Consecutive Fibonacci numbers exercise Euclid; the other pair has gcd 2.
    i64 first = in.shape ? 2000000014LL : 1134903170LL;
    i64 second = in.shape ? 2000000018LL : 1836311903LL;
    i64 mod = std::lcm(first, second);
    std::vector<std::array<i64, 3>> queries;
    for (int i = 0; i < in.n; ++i) {
        i64 answer = i64(random() % mod);
        i64 a = answer % first, b = answer % second;
        if (in.shape and i % 2) {
            b = (b + 1) % second;
            answer = -1;
        }
        queries.push_back({a, b, answer});
    }
    return measure(in, "CRT batches: coprime Fibonacci moduli / compatible and conflicting gcd-2 pairs",
                   [&]() -> std::uint64_t {
        std::uint64_t sum = 0;
        for (auto [a, b, expected] : queries) {
            ExCrt<i64> crt;
            crt.insert(a, first);
            crt.insert(b, second);
            i64 value = crt.get();
            benchmarkCheck(value == expected, "known least residue / incompatible parity");
            sum = benchmarkMix(sum, std::uint64_t(value));
        }
        return sum;
    });
}
