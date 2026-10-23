#include "../../../../../src/extra/Math/RangePrimeCount/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("RangePrimeCount/oracle", [] {
int cnt = 0;
        CHECK(cntPrimeLessEqual(-1) == 0);
        for (int n = 0; n <= 1200; ++n) {
            bool prime = n >= 2;
            for (int d = 2; d * d <= n; ++d) {
                if (n % d == 0) { prime = false; }
            }
            cnt += prime;
            CHECK(cntPrimeLessEqual(n) == cnt);
        }
        CHECK(cntPrimeLessEqual(1000000) == 78498);
    });
    return 0;
}
