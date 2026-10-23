#include "../../../../../src/extra/DataStructures/RangeMatBIT/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("RangeMatBIT/oracle", [] {
RangeMatBIT<long long> t(7,8);
        std::vector<std::vector<long long>> a(7,std::vector<long long>(8));
        for (int k = 0; k < 150; ++k) {
            int u = randomInt(0,7), d = randomInt(u,7), l = randomInt(0,8), r = randomInt(l,8), v = randomInt(-5,5);
            t.rangeModify(u,l,d,r,v);
            for (int i = u; i < d; ++i) { for (int j = l; j < r; ++j) { a[i][j] += v; } }
            u = randomInt(0,7); d = randomInt(u,7); l = randomInt(0,8); r = randomInt(l,8);
            long long want = 0;
            for (int i = u; i < d; ++i) { for (int j = l; j < r; ++j) { want += a[i][j]; } }
            CHECK(t.rangeQuery(u,l,d,r) == want);
        }
        RangeMatBIT<long long> empty(0,0);
        empty.rangeModify(0,0,0,0,1);
        CHECK(empty.rangeQuery(0,0,0,0) == 0);
    });
    return 0;
}
