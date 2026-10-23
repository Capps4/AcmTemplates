#include "../../../../../src/extra/DataStructures/MatBIT/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("MatBIT/oracle", [] {
MatBIT<long long> t(8,9);
        std::vector<std::vector<long long>> a(8,std::vector<long long>(9));
        CHECK(t.posQuery(-1,-1) == 0);
        for (int k = 0; k < 200; ++k) {
            int x = randomInt(0,7), y = randomInt(0,8), v = randomInt(-10,10);
            t.modify(x,y,v); a[x][y] += v;
            int u = randomInt(0,8), d = randomInt(u,8), l = randomInt(0,9), r = randomInt(l,9);
            long long want = 0;
            for (int i = u; i < d; ++i) { for (int j = l; j < r; ++j) { want += a[i][j]; } }
            CHECK(t.rangeQuery(u,l,d,r) == want);
        }
        MatBIT<long long> empty(0,0);
        CHECK(empty.rangeQuery(0,0,0,0) == 0);
    });
    return 0;
}
