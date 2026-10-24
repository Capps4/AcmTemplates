#include "../../../../../src/extra/DataStructures/SparseMatBIT/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

int main() {
    runCase("SparseMatBIT/corners", [] {
        SparseMatBIT<long long> empty({});
        CHECK(empty.prefixQuery(0, 0) == 0);
        CHECK(empty.rangeQuery(-10, -10, 10, 10) == 0);
        const long long lo = std::numeric_limits<long long>::lowest();
        const long long hi = std::numeric_limits<long long>::max();
        SparseMatBIT<long long, long long> t({{lo, lo}, {hi, hi}, {0, 0}, {0, 0}});
        t.modify(lo, lo, 3);
        t.modify(hi, hi, 7);
        t.modify(0, 0, 5);
        t.modify(0, 0, -2);
        CHECK(t.prefixQuery(lo, hi) == 0);
        CHECK(t.prefixQuery(0, 0) == 3);
        CHECK(t.prefixQuery(hi, hi) == 6);
        CHECK(t.rangeQuery(lo, lo, 0, 0) == 3);
        CHECK(t.rangeQuery(0, 0, hi, hi) == 3);
        CHECK(t.rangeQuery(lo, 0, hi, 0) == 0);
        CHECK(t.rangeQuery(hi, hi, hi, hi) == 0);
    });
    runCase("SparseMatBIT/oracle", [] {
        for (int trial = 0; trial < 30; ++trial) {
            std::vector<std::pair<int, int>> points;
            std::map<std::pair<int, int>, long long> values;
            for (int i = 0; i < 30; ++i) {
                auto point = std::pair{randomInt(-6, 6), randomInt(-6, 6)};
                points.push_back(point);
                values[point] = 0;
            }
            SparseMatBIT<long long> t(points);
            for (int step = 0; step < 200; ++step) {
                test_context::step = step;
                auto point = points[randomInt(0, int(points.size()) - 1)];
                int delta = randomInt(-20, 20);
                t.modify(point.first, point.second, delta);
                values[point] += delta;
                int xl = randomInt(-9, 9), xr = randomInt(-9, 9);
                int yl = randomInt(-9, 9), yr = randomInt(-9, 9);
                if (xl > xr) {
                    std::swap(xl, xr);
                }
                if (yl > yr) {
                    std::swap(yl, yr);
                }
                long long rectangle = 0, prefix = 0;
                for (const auto& [p, value] : values) {
                    prefix += p.first < xr and p.second < yr ? value : 0;
                    if (xl <= p.first and p.first < xr and yl <= p.second and p.second < yr) {
                        rectangle += value;
                    }
                }
                CHECK(t.prefixQuery(xr, yr) == prefix);
                CHECK(t.rangeQuery(xl, yl, xr, yr) == rectangle);
            }
        }
    });
    runCase("SparseMatBIT/sparse-large", [] {
        // Debug lower_bound validates partitions linearly; keep this structural
        // case modest and measure the 10K-point workload in optimized benchmarks.
        constexpr int n = 512;
        std::vector<std::pair<long long, long long>> points;
        for (int i = 0; i < n; ++i) {
            points.push_back({1000000000LL * i, -1000000000LL * i});
        }
        SparseMatBIT<long long, long long> t(points);
        for (const auto& [x, y] : points) {
            t.modify(x, y, 1);
        }
        std::set<int> probes{0, n - 1};
        for (int i = 1; i < n; i *= 2) {
            probes.insert(i - 1);
            probes.insert(i);
            probes.insert(std::min(n - 1, i + 1));
        }
        for (int i = 0; i < n; i += 17) {
            probes.insert(i);
        }
        for (int i : probes) {
            auto [x, y] = points[i];
            CHECK(t.rangeQuery(x, y, x + 1, y + 1) == 1);
        }
        CHECK(t.rangeQuery(0, -1000000000LL * n, 1000000000LL * n, 1) == n);
    });
    return 0;
}
