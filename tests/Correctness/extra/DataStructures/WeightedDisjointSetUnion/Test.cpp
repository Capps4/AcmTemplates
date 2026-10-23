#include "../../../../../src/extra/DataStructures/WeightedDisjointSetUnion/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("WeightedDisjointSetUnion/oracle", [] {
        auto exercise = [](const std::vector<long long>& pot, bool structured) {
            int n = int(pot.size());
            WeightedDSU<long long> d(n);
            std::vector<std::vector<std::pair<int, long long>>> graph(n);
            auto difference = [&](int start, int finish) -> std::optional<long long> {
                std::vector<int> seen(n, 0), queue{start};
                std::vector<long long> dist(n, 0);
                seen[start] = 1;
                for (std::size_t i = 0; i < queue.size(); ++i) {
                    int x = queue[i];
                    if (x == finish) {
                        return dist[x];
                    }
                    for (const auto& [y, weight] : graph[x]) {
                        if (!seen[y]) {
                            seen[y] = 1;
                            dist[y] = dist[x] + weight;
                            queue.push_back(y);
                        }
                    }
                }
                return std::nullopt;
            };
            auto join = [&](int x, int y, long long delta) {
                auto known = difference(x, y);
                bool expected = !known or *known == delta;
                CHECK(d.Union(x, y, delta) == expected);
                if (!known) {
                    graph[x].push_back({y, delta});
                    graph[y].push_back({x, -delta});
                }
            };
            auto link = [&](int x, int y) { join(x, y, pot[y] - pot[x]); };
            auto check = [&](bool compress) {
                std::vector<int> roots(n);
                std::vector<long long> sums(n, 0);
                int maxDepth = 0;
                for (int x = 0; x < n; ++x) {
                    int at = x, depth = 0;
                    // Observe the uncompressed parent path before invoking find.
                    while (d.f[at] != at) {
                        CHECK(depth < n);
                        sums[x] += d.w[at];
                        at = d.f[at];
                        CHECK(0 <= at and at < n);
                        ++depth;
                    }
                    roots[x] = at;
                    maxDepth = std::max(maxDepth, depth);
                    auto expected = difference(at, x);
                    CHECK(expected and sums[x] == *expected);
                }
                for (int x = 0; x < n; ++x) {
                    for (int y = 0; y < n; ++y) {
                        auto expected = difference(x, y);
                        CHECK((roots[x] == roots[y]) == bool(expected));
                        if (expected) {
                            CHECK(sums[y] - sums[x] == *expected);
                            CHECK(*expected == pot[y] - pot[x]);
                        }
                    }
                }
                if (compress) {
                    for (int x = 0; x < n; ++x) {
                        CHECK(d.find(x) == roots[x]);
                        CHECK(d.w[x] == sums[x]);
                    }
                    for (int x = n - 1; x >= 0; --x) {
                        CHECK(d.find(x) == roots[x]);
                        CHECK(d.w[x] == sums[x]);
                    }
                }
                return maxDepth;
            };

            check(false);
            if (structured) {
                link(0, 1);
                link(2, 3);
                link(4, 5);
                link(2, 4);
                CHECK(check(false) >= 2);
                link(1, 3); // Size 2 -> size 4, with both endpoints non-root.
                CHECK(check(false) >= 2);
                link(6, 7);
                link(8, 9);
                link(6, 8);
                link(7, 5); // Size 4 -> size 6, including a depth-two endpoint.
                link(10, 11);
                link(9, 11); // Larger first: exercise the opposite attachment.
                CHECK(check(false) >= 2);
            } else {
                for (int i = 0; i < 2 * n; ++i) {
                    link(randomInt(0, n - 1), randomInt(0, n - 1));
                }
                check(false);
                for (int x = 1; x < n; ++x) {
                    link(x, 0);
                }
            }
            check(true);
            for (int x = 0; x < n; ++x) {
                for (int y = 0; y < n; ++y) {
                    // Includes self constraints, reverse endpoints and non-root nodes.
                    link(x, y);
                    join(x, y, pot[y] - pot[x] + 1);
                    join(y, x, pot[x] - pot[y] - 1);
                }
            }
            check(true);
        };

        exercise({0, 13, -7, 21, -35, 8, -5, 44, -17, 2, 31, -29}, true);
        for (int it = 0; it < 20; ++it) {
            std::vector<long long> pot(20);
            for (auto& value : pot) {
                value = randomInt(-100, 100);
            }
            exercise(pot, false);
        }
    });
    return 0;
}
