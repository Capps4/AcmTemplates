#include "../../../../../src/extra/Math/DiscreteLog/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

using i64 = long long;

bool prime(int p) {
    for (int d = 2; i64(d) * d <= p; ++d) {
        if (p % d == 0)
            return false;
    }
    return p >= 2;
}

i64 power(i64 a, int k, int m) {
    i64 value = 1;
    while (k > 0) {
        if (k % 2)
            value = value * a % m;
        a = a * a % m;
        k /= 2;
    }
    return value;
}

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    benchmarkCheck(input.n <= 20000, "benchmark modulus fits int");
    int p = std::max(3, input.n * input.n + 1);
    while (not prime(p))
        ++p;
    int m = input.shape ? 2 * p : p;
    int rem = p - 1;
    std::vector<int> factors;
    for (int d = 2; i64(d) * d <= rem; ++d) {
        if (rem % d != 0)
            continue;
        factors.push_back(d);
        while (rem % d == 0)
            rem /= d;
    }
    if (rem > 1)
        factors.push_back(rem);
    int root = 1;
    while (true) {
        bool full = true;
        for (int q : factors) {
            if (power(root, (p - 1) / q, p) == 1)
                full = false;
        }
        if (full)
            break;
        ++root;
    }
    std::mt19937_64 rng(input.seed);
    std::vector<std::pair<int, int>> queries;
    std::uint64_t target = 0;
    for (int i = 0; i < 16; ++i) {
        int e = 1 + rng() % (p - 1);
        while (std::gcd(e, p - 1) != 1)
            e = 1 + rng() % (p - 1);
        int a = int(power(root, e, p));
        if (input.shape and a % 2 != 0)
            a += p;
        // The verified prime factors establish order p-1 modulo p.
        // For modulus 2p the even lift also excludes the exceptional exponent 0.
        int expected = 1 + rng() % (p - 2);
        i64 b = power(a, expected, m);
        queries.emplace_back(a, int(b));
        target = benchmarkMix(target, expected);
    }
    return measure(input, "16 discrete logs near n^2: coprime / non-coprime", [&] {
        std::uint64_t sum = 0;
        for (auto [a, b] : queries)
            sum = benchmarkMix(sum, discreteLog(a, b, m));
        benchmarkCheck(sum == target, "verified full multiplicative order and minimal exponents");
        return sum;
    });
}
