#include "../../../../../src/extra/DataStructures/RollbackDSU/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

std::vector<int> components(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<int>> graph(n);
    for (auto [x, y] : edges) {
        graph[x].push_back(y);
        graph[y].push_back(x);
    }
    std::vector<int> ids(n, -1);
    for (int s = 0; s < n; ++s) {
        if (ids[s] != -1) {
            continue;
        }
        std::vector<int> queue{s};
        ids[s] = s;
        for (std::size_t i = 0; i < queue.size(); ++i) {
            for (int y : graph[queue[i]]) {
                if (ids[y] == -1) {
                    ids[y] = s;
                    queue.push_back(y);
                }
            }
        }
    }
    return ids;
}

int main() {
    runCase("RollbackDSU/corners", [] {
        RollbackDSU empty(0);
        CHECK(empty.snapshot() == 0);
        empty.rollback(0);
        RollbackDSU d(5);
        CHECK(not d.Union(0, 0));
        CHECK(d.snapshot() == 0);
        CHECK(d.Union(1, 2));
        auto s = d.snapshot();
        CHECK(not d.Union(2, 1));
        CHECK(d.snapshot() == s);
        CHECK(d.Union(0, 2));
        CHECK(d.size(0) == 3);
        CHECK(d.Union(3, 4));
        CHECK(d.Union(4, 0));
        CHECK(d.size(3) == 5);
        d.rollback(s);
        d.rollback(s);
        CHECK(d.same(1, 2));
        CHECK(d.size(1) == 2);
        for (int x : {0, 3, 4}) {
            CHECK(d.size(x) == 1);
        }
        CHECK(d.Union(2, 4));
        CHECK(not d.same(3, 4));
        d.rollback(0);
        for (int x = 0; x < 5; ++x) {
            CHECK(d.size(x) == 1);
            CHECK(d.find(x) == x);
        }
    });
    runCase("RollbackDSU/oracle", [] {
        constexpr int n = 9;
        RollbackDSU d(n);
        std::vector<std::pair<int, int>> edges;
        std::vector<std::pair<std::size_t, std::size_t>> marks{{0, 0}};
        for (int step = 0; step < 1200; ++step) {
            test_context::step = step;
            int action = randomInt(0, 4);
            if (action == 0) {
                marks.push_back({d.snapshot(), edges.size()});
            } else if (action == 1) {
                int at = randomInt(0, int(marks.size()) - 1);
                d.rollback(marks[at].first);
                edges.resize(marks[at].second);
                marks.resize(at + 1);
            } else {
                auto ids = components(n, edges);
                int x = randomInt(0, n - 1), y = randomInt(0, n - 1);
                bool added = ids[x] != ids[y];
                CHECK(d.Union(x, y) == added);
                if (added) {
                    edges.push_back({x, y});
                }
            }
            auto ids = components(n, edges);
            for (int x = 0; x < n; ++x) {
                int count = 0;
                for (int y = 0; y < n; ++y) {
                    CHECK(d.same(x, y) == (ids[x] == ids[y]));
                    count += ids[x] == ids[y];
                }
                CHECK(d.size(x) == count);
            }
        }
    });
    runCase("RollbackDSU/balanced-large", [] {
        constexpr int n = 1 << 17;
        RollbackDSU d(n);
        for (int len = 1; len < n; len *= 2) {
            for (int x = 0; x < n; x += 2 * len) {
                CHECK(d.Union(x, x + len));
            }
        }
        for (int x = 0; x < n; ++x) {
            CHECK(d.same(0, x));
            CHECK(d.size(x) == n);
        }
        CHECK(d.snapshot() == n - 1);
        d.rollback(0);
        for (int x = 0; x < n; ++x) {
            CHECK(d.size(x) == 1);
        }
    });
    return 0;
}
