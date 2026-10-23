#include "../../../../../src/extra/Graph/BiGraphColor/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("BiGraphColor/oracle", [] {
for (int it = 0; it < 100; ++it) {
            int n = randomInt(0,8);
            std::vector<std::vector<int>> g(n);
            for (int x = 0; x < n; ++x) {
                for (int y = x; y < n; ++y) {
                    if (randomInt(0,5) == 0) { g[x].push_back(y); g[y].push_back(x); }
                }
            }
            bool want = false;
            for (int s = 0; s < (1 << n); ++s) {
                bool ok = true;
                for (int x = 0; x < n; ++x) {
                    for (int y : g[x]) { ok = ok and (((s >> x) ^ (s >> y)) & 1); }
                }
                want = want or ok;
            }
            BiGraphColor a(g);
            CHECK(a.isBiGraph == want);
            if (want) {
                for (int x = 0; x < n; ++x) {
                    for (int y : g[x]) { CHECK(a.col[x] != a.col[y]); }
                }
            }
        }
    });
    return 0;
}
