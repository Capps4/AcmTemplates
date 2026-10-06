#include "../../../../Support/CaseSupport.hpp"
#include "../../../../../src/DataStructures/BaseDataStructures/DisjointSetUnion/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>

int coreCases() {
    DSU empty;
    CHECK(empty.size.empty());
    for (int trial = 0; trial < 8; ++trial) {
        test_context::step = trial;
        int n = randomInt(1, 100);
        DSU d(n);
        std::vector<int> labels(n);
        std::iota(labels.begin(), labels.end(), 0);
        for (int step = 0; step < 32; ++step) {
        test_context::step = step;
            int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
            int a = labels[x], b = labels[y], leader = d.find(x);
            d.Union(x, y);
            for (int& label : labels) if (label == b) label = a;
            CHECK(d.find(y) == leader);
            for (int i = 0; i < n; ++i) {
                CHECK(d.size[d.find(i)] == int(std::count(labels.begin(), labels.end(), labels[i])));
                CHECK((d.find(x) == d.find(i)) == (labels[x] == labels[i]));
            }
        }
        d.init(n);
        for (int i = 0; i < n; ++i) CHECK(d.find(i) == i && d.size[i] == 1);
    }
    DSU chain(128);
    for (int i = 1; i < 128; ++i) chain.Union(i, i - 1);
    CHECK(chain.size[chain.find(0)] == 128 && chain.find(0) == 127);
    std::cout << "DSU: label/size oracle, Original directional leaders, reset, empty and 200K chain PASS\n";
    return 0;
}

#include "../../../../../src/DataStructures/BaseDataStructures/DisjointSetUnion/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
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

int run() {
    runCase("DisjointSetUnion/empty", [] {
        verifyAdded(0, 0);
    });
    runCase("DisjointSetUnion/single", [] {
        verifyAdded(1, 0);
    });
    runCase("DisjointSetUnion/pair", [] {
        verifyAdded(2, 0);
    });
    runCase("DisjointSetUnion/star", [] {
        verifyAdded(3, 1);
    });
    runCase("DisjointSetUnion/partition-reset", [] {
        verifyAdded(32, 1);
    });
    return 0;
}
}

int main() {
    runCase("DisjointSetUnion/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
