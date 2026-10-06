#include "../../../../Support/CaseSupport.hpp"
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
int coreCases() {
    for (int capacity : {0, 1, 2, 3, 10, 1000}) {
        Sieve sieve(capacity);
        auto oldSize = sieve.size();
        sieve.init(capacity); sieve.init(0);
        CHECK(sieve.size() == oldSize);
        for (int x = 1; x <= 64; ++x) {
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
    for (int test = 0; test < 16; ++test) {
        test_context::step = test;
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
    return 0;
}

#include "../../../../../src/Math/MathPackage/Sieve/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
std::vector<std::pair<long long, int>> trial(long long x) {
    std::vector<std::pair<long long, int>> result;
    for (long long p = 2; p <= x / p; ++p)
        if (x % p == 0) {
            int count = 0;
            do {
                x /= p;
                ++count;
            } while (x % p == 0);
            result.emplace_back(p, count);
        }
    if (x > 1)
        result.emplace_back(x, 1);
    return result;
}

int run() {
    runCase("Sieve/unit", [] {
        Sieve s(0);
        CHECK(s.primeFactorize(1LL) == trial(1LL));
        auto a = s.allFactors(1LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 1LL; ++d)
            if (1LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/first-prime", [] {
        Sieve s(0);
        CHECK(s.primeFactorize(2LL) == trial(2LL));
        auto a = s.allFactors(2LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 2LL; ++d)
            if (2LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/square", [] {
        Sieve s(1);
        CHECK(s.primeFactorize(49LL) == trial(49LL));
        auto a = s.allFactors(49LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 49LL; ++d)
            if (49LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/prime-power", [] {
        Sieve s(3);
        CHECK(s.primeFactorize(1024LL) == trial(1024LL));
        auto a = s.allFactors(1024LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 1024LL; ++d)
            if (1024LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/many-factors", [] {
        Sieve s(10);
        CHECK(s.primeFactorize(2520LL) == trial(2520LL));
        auto a = s.allFactors(2520LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 2520LL; ++d)
            if (2520LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/growth-prime", [] {
        Sieve s(0);
        CHECK(s.primeFactorize(9973LL) == trial(9973LL));
        auto a = s.allFactors(9973LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 9973LL; ++d)
            if (9973LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/capacity-crossing", [] {
        Sieve s(31);
        CHECK(s.primeFactorize(1025LL) == trial(1025LL));
        auto a = s.allFactors(1025LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 1025LL; ++d)
            if (1025LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/odd-composite", [] {
        Sieve s(2);
        CHECK(s.primeFactorize(945LL) == trial(945LL));
        auto a = s.allFactors(945LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 945LL; ++d)
            if (945LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/semiprime", [] {
        Sieve s(11);
        CHECK(s.primeFactorize(10403LL) == trial(10403LL));
        auto a = s.allFactors(10403LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 10403LL; ++d)
            if (10403LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    runCase("Sieve/perfect-square", [] {
        Sieve s(100);
        CHECK(s.primeFactorize(10000LL) == trial(10000LL));
        auto a = s.allFactors(10000LL);
        std::sort(a.begin(), a.end());
        std::vector<long long> b;
        for (long long d = 1; d <= 10000LL; ++d)
            if (10000LL % d == 0)
                b.push_back(d);
        CHECK(a == b);
    });
    return 0;
}
}

int main() {
    runCase("Sieve/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
