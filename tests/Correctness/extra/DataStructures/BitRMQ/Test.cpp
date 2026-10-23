#include "../../../../../src/extra/DataStructures/BitRMQ/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("BitRMQ/oracle", [] {
BitRMQ<int> empty({});
        (void)empty;
        for (int n : {1,2,63,64,65,127,128,129,193}) {
            std::vector<int> a(n);
            for (auto& x : a) { x = randomInt(-5,5); }
            BitRMQ<int> mn(a);
            BitRMQ<int,std::greater<int>> mx(a);
            for (int l = 0; l < n; ++l) {
                int lo = a[l], hi = a[l];
                for (int r = l+1; r <= n; ++r) {
                    lo = std::min(lo,a[r-1]); hi = std::max(hi,a[r-1]);
                    CHECK(mn(l,r) == lo and mx(l,r) == hi);
                }
            }
        }
    });
    return 0;
}
