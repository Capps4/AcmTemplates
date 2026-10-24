#include "../../../../../src/extra/Math/RangePrimeCount/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("RangePrimeCount/oracle", [] {
        int cnt = 0;
        CHECK(cntPrimeLessEqual(-1) == 0);
        CHECK(cntPrimeLessEqual(std::numeric_limits<long long>::min()) == 0);
        for (int n = 0; n <= 1200; ++n) {
            bool prime = n >= 2;
            for (int d = 2; d * d <= n; ++d) {
                if (n % d == 0) { prime = false; }
            }
            cnt += prime;
            CHECK(cntPrimeLessEqual(n) == cnt);
        }
        // Test both sqrt-table branches at selected squares with a sieve oracle.
        std::vector<bool> prime(10002, true);
        prime[0] = prime[1] = false;
        for (int p = 2; p * p < int(prime.size()); ++p)
            if (prime[p])
                for (int x = p * p; x < int(prime.size()); x += p)
                    prime[x] = false;
        for (int root : {31, 32, 99, 100})
            for (int delta : {-1, 0, 1}) {
                int n = root * root + delta;
                auto expected = std::count(prime.begin(), prime.begin() + n + 1, true);
                CHECK(cntPrimeLessEqual(n) == expected);
            }
        for (int n : {999999, 1000000, 1000001})
            CHECK(cntPrimeLessEqual(n) == 78498);
    });
    return 0;
}
