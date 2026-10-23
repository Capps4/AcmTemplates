#include "../../../../../src/extra/Tree/DecareTree/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("DecareTree/oracle", [] {
DecareTree<int> empty({});
        CHECK(empty.root == -1);
        for (int it = 0; it < 30; ++it) {
            std::vector<int> a(30);
            for (auto& x : a) { x = randomInt(0,5); }
            DecareTree<int> t(a);
            auto rec = [&](auto&& self, int l, int r, int x) -> void {
                if (l == r) { CHECK(x == -1); return; }
                int p = l;
                for (int i = l+1; i < r; ++i) {
                    if (a[i] <= a[p]) { p = i; }
                }
                CHECK(x == p);
                self(self,l,p,t.t[x].first);
                self(self,p+1,r,t.t[x].second);
            };
            rec(rec,0,int(a.size()),t.root);
        }
    });
    return 0;
}
