#include "Final.hpp"
#include "../TestSupport.hpp"
#include <algorithm>
#include <climits>
#include <tuple>
#include <vector>

int main() {
    for (int n : {0, 1, 2, 100, 100000, INT_MAX}) {
        Query::rangeScale(n);
        int blockSize = int(std::sqrt(2.0 * n)) + 1;
        std::vector<Query> queries;
        for (int i = 0; i < 3000; ++i) {
            int l = randomInt(0, n), r = randomInt(l, n);
            queries.emplace_back(l, r, i);
        }
        auto key = [&](const Query& q) {
            int block = q.l / blockSize;
            return std::pair{block, block % 2 == 0 ? q.r : -q.r};
        };
        std::sort(queries.begin(), queries.end());
        for (int i = 0; i < int(queries.size()); ++i) {
            CHECK(!(queries[i] < queries[i]));
            if (i) CHECK(key(queries[i - 1]) <= key(queries[i]));
        }
        for (int trial = 0; trial < 5000; ++trial) {
            const auto& a = queries[randomInt(0, 2999)];
            const auto& b = queries[randomInt(0, 2999)];
            CHECK((a < b) == (key(a) < key(b)));
        }
    }
    std::cout << "Snake-order oracle, strict weak order and INT_MAX scaling passed\n";
}
