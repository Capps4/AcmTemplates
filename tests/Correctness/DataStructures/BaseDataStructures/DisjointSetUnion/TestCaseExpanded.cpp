#include "../../../../../src/DataStructures/BaseDataStructures/DisjointSetUnion/code.hpp"
#include "../../../../Support/CaseSupport.hpp"
void verifyAdded(int n, int shape) {
    DSU d(n);
    std::vector<int> p(n);
    std::iota(p.begin(), p.end(), 0);
    for (int i = 0; i < n; ++i) {
        int a = shape ? 0 : i, b = (i * 7 + 3) % n, old = p[b], to = p[a];
        d.Union(a, b);
        for (auto &x : p)
            if (x == old)
                x = to;
        for (int u = 0; u < n; ++u) {
            CHECK(d.size[d.find(u)] == int(std::count(p.begin(), p.end(), p[u])));
            for (int v = 0; v < n; ++v)
                CHECK((d.find(u) == d.find(v)) == (p[u] == p[v]));
        }
    }
    d.init(n);
    for (int i = 0; i < n; ++i)
        CHECK(d.size[d.find(i)] == 1);
}

int main() {
    runCase("DisjointSetUnion/01-partition-00", [] {
        verifyAdded(0, 0);
    });
    runCase("DisjointSetUnion/02-partition-01", [] {
        verifyAdded(1, 0);
    });
    runCase("DisjointSetUnion/03-partition-02", [] {
        verifyAdded(2, 0);
    });
    runCase("DisjointSetUnion/04-partition-03", [] {
        verifyAdded(3, 1);
    });
    runCase("DisjointSetUnion/05-partition-04", [] {
        verifyAdded(4, 0);
    });
    runCase("DisjointSetUnion/06-partition-05", [] {
        verifyAdded(5, 1);
    });
    runCase("DisjointSetUnion/07-partition-06", [] {
        verifyAdded(7, 0);
    });
    runCase("DisjointSetUnion/08-partition-07", [] {
        verifyAdded(8, 1);
    });
    runCase("DisjointSetUnion/09-partition-08", [] {
        verifyAdded(17, 0);
    });
    runCase("DisjointSetUnion/10-partition-09", [] {
        verifyAdded(32, 1);
    });
    return finishCases(10);
}
