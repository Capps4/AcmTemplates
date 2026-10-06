#include "../../../../../src/DataStructures/BaseDataStructures/DisjointSetUnion/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <algorithm>
int main() {
    DSU empty;
    CHECK(empty.size.empty());
    for (int trial = 0; trial < 100; ++trial) {
        int n = randomInt(1, 100);
        DSU d(n);
        std::vector<int> labels(n);
        std::iota(labels.begin(), labels.end(), 0);
        for (int step = 0; step < 1000; ++step) {
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
    DSU chain(200000);
    for (int i = 1; i < 200000; ++i) chain.Union(i, i - 1);
    CHECK(chain.size[chain.find(0)] == 200000 && chain.find(0) == 199999);
    std::cout << "DSU: label/size oracle, Original directional leaders, reset, empty and 200K chain PASS\n";
}
