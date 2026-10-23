#include "../../../../../src/extra/DataStructures/RangeBIT/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("RangeBIT/oracle", [] {
RangeBIT<long long> empty(0);
        empty.rangeModify(0,0,5);
        CHECK(empty.rangeQuery(0,0) == 0);
        for (int it = 0; it < 10; ++it) {
            std::vector<long long> a(40);
            for (auto& x : a) { x = randomInt(-10,10); }
            RangeBIT<long long> t(a);
            for (int k = 0; k < 100; ++k) {
                int l = randomInt(0,40), r = randomInt(l,40), v = randomInt(-5,5);
                t.rangeModify(l,r,v);
                for (int i = l; i < r; ++i) { a[i] += v; }
                l = randomInt(0,40); r = randomInt(l,40);
                CHECK(t.rangeQuery(l,r) == std::accumulate(a.begin()+l,a.begin()+r,0LL));
            }
        }
    });
    return 0;
}
