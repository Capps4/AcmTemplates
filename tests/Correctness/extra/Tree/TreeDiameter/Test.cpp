#include "../../../../../src/extra/Tree/TreeDiameter/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("TreeDiameter/oracle", [] {
std::vector<std::vector<int>> empty;
        TreeDiameter<int> z(empty);
        CHECK(z.diameter == 0 and z.v1 == -1 and z.v2 == -1);
        for (int it = 0; it < 30; ++it) {
            int n = randomInt(1,25);
            std::vector<std::vector<std::pair<int,int>>> g(n);
            std::vector<std::vector<long long>> d(n,std::vector<long long>(n,1000000));
            for (int x = 0; x < n; ++x) { d[x][x] = 0; }
            for (int x = 1; x < n; ++x) {
                int p = randomInt(0,x-1), w = randomInt(0,20);
                g[x].emplace_back(p,w); g[p].emplace_back(x,w); d[x][p] = d[p][x] = w;
            }
            for (int k = 0; k < n; ++k) { for (int x = 0; x < n; ++x) { for (int y = 0; y < n; ++y) { d[x][y] = std::min(d[x][y],d[x][k]+d[k][y]); } } }
            long long want = 0;
            for (const auto& row : d) { for (auto v : row) { want = std::max(want,v); } }
            TreeDiameter<std::pair<int,int>> t(g);
            CHECK(t.diameter == want and d[t.v1][t.v2] == want);
        }
    });
    return 0;
}
