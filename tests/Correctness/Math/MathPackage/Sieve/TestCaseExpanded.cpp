#include "../../../../../src/Math/MathPackage/Sieve/code.hpp"
#include "../../../../Support/TestSupport.hpp"
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
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("Sieve/01-unit", [] {
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
    runCase("Sieve/02-first-prime", [] {
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
    runCase("Sieve/03-square", [] {
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
    runCase("Sieve/04-prime-power", [] {
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
    runCase("Sieve/05-many-factors", [] {
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
    runCase("Sieve/06-growth-prime", [] {
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
    runCase("Sieve/07-capacity-crossing", [] {
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
    runCase("Sieve/08-odd-composite", [] {
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
    runCase("Sieve/09-semiprime", [] {
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
    runCase("Sieve/10-perfect-square", [] {
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
    return finishCases(10);
}
