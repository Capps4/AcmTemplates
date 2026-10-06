#include "../../../../../src/Math/MathPackage/Sieve/code.hpp"
#include "../../../../Support/TestSupport.hpp"
std::vector<std::pair<long long, int>> trial(long long x) {
    std::vector<std::pair<long long, int>> result;
    for (long long p = 2; p <= x / p; ++p) if (x % p == 0) {
        int count = 0;
        do { x /= p; ++count; } while (x % p == 0);
        result.emplace_back(p, count);
    }
    if (x > 1) result.emplace_back(x, 1);
    return result;
}
int main() {
    for (int capacity : {0, 1, 2, 3, 10, 1000}) {
        Sieve sieve(capacity);
        auto oldSize = sieve.size();
        sieve.init(capacity); sieve.init(0);
        CHECK(sieve.size() == oldSize);
        for (int x = 1; x <= 3000; ++x) {
            CHECK(sieve.primeFactorize(1LL * x) == trial(x));
            auto actual = sieve.allFactors(x);
            std::sort(actual.begin(), actual.end());
            std::vector<int> expected;
            for (int d = 1; d <= x; ++d) if (x % d == 0) expected.push_back(d);
            CHECK(actual == expected);
        }
        for (int x : {0, 1, 2, 3, 4, 31, 1000, 4000}) {
            CHECK(sieve.mpf(x) == (x < 2 ? 0 : trial(x).front().first));
            CHECK(sieve.size() > std::size_t(x));
        }
    }
    Sieve sieve(100000);
    CHECK(sieve.primes().size() == 9592);
    for (int test = 0; test < 10000; ++test) {
        long long x = randomInt(1, 100000000);
        CHECK(sieve.primeFactorize(x) == trial(x));
    }
    using U = unsigned long long;
    std::vector<std::pair<U,int>> expected{{3,1},{5,1},{17,1},{257,1},{641,1},{65537,1},{6700417,1}};
    CHECK(sieve.primeFactorize(std::numeric_limits<U>::max()) == expected);
    auto divisors = sieve.allFactors(std::numeric_limits<U>::max());
    CHECK(divisors.size() == 128);
    CHECK(*std::max_element(divisors.begin(), divisors.end()) == std::numeric_limits<U>::max());
    CHECK(sieve.primeFactorize(static_cast<unsigned char>(255)) == (std::vector<std::pair<unsigned char,int>>{{3,1},{5,1},{17,1}}));
    static_assert(std::is_reference_v<decltype(siv)>);
    CHECK(&siv == &Sieve::shared());
    std::cout << "Sieve: trial oracle, divisors, boundaries, no-op init, full uint64 PASS\n";
}
