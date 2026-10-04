#include <bits/stdc++.h>
#include "/Users/bytedance/acm_compete/cpp17/SparseTable/Final.hpp"
#include "/Users/bytedance/acm_compete/cpp17/BenchmarkSupport.hpp"
namespace Legacy {
#include "/Users/bytedance/acm_compete/cpp17/SparseTable/Original.hpp"
}
struct CapturedCmp {
    std::vector<int> thresholds = std::vector<int>(256, 0);
    bool operator()(int a, int b) const { return a + thresholds[0] < b + thresholds[0]; }
};
namespace Before {
// Immutable range extremum queries, 0-based [l,r).
template<class T, class Cmp = std::less<T>>
class RMQ {
    Cmp cmp;
    std::vector<std::vector<T>> jump;
    static int floorLog(unsigned value) {
        assert(value > 0);
        return std::numeric_limits<unsigned>::digits - 1 - __builtin_clz(value);
    }
    const T& better(const T& a, const T& b) const { return cmp(b, a) ? b : a; }
public:
    explicit RMQ(const std::vector<T>& a, Cmp cmp = {}) : cmp(std::move(cmp)) {
        if (a.empty()) return;
        int n = int(a.size()), logn = floorLog(unsigned(n));
        jump.resize(logn + 1);
        jump[0] = a;
        for (int level = 1; level <= logn; ++level) {
            int width = 1 << level, half = width / 2;
            int count = n - width + 1;
            if constexpr (std::is_default_constructible_v<T>) {
                jump[level].resize(count);
                for (int i = 0; i < count; ++i)
                    jump[level][i] = better(jump[level - 1][i], jump[level - 1][i + half]);
            } else {
                jump[level].reserve(count);
                for (int i = 0; i < count; ++i)
                    jump[level].push_back(better(jump[level - 1][i], jump[level - 1][i + half]));
            }
        }
    }
    // Borrowed reference valid for the lifetime of this unchanged RMQ.
    const T& operator()(int l, int r) const {
        assert(0 <= l && l < r && !jump.empty() && r <= int(jump[0].size()));
        int log = floorLog(unsigned(r - l));
        return better(jump[log][l], jump[log][r - (1 << log)]);
    }
};

}
#undef call
int main() {
    std::mt19937 rng(20261001);
    std::vector<int> values(100000);
    for (int& x : values) x = rng() % 1000000;
    compare("build-100K", [&] { Before::RMQ<int> tree(values); return std::uint64_t(tree(0, values.size())); },
                          [&] { RMQ tree(values); return std::uint64_t(tree(0, values.size())); });
    std::vector<std::pair<int, int>> queries(300000);
    for (auto& [l, r] : queries) { l = rng() % values.size(); r = l + 1 + rng() % (values.size() - l); }
    Before::RMQ<int> oldTree(values);
    RMQ tree(values);
    compare("query-300K", [&] { std::uint64_t sum = 0; for (auto [l, r] : queries) sum += oldTree(l, r); return sum; },
                          [&] { std::uint64_t sum = 0; for (auto [l, r] : queries) sum += tree(l, r); return sum; });
    values.resize(30000);
    compare("build-stateful-comparator-30K", [&] {
        Before::RMQ<int, CapturedCmp> table(values); return std::uint64_t(table(0, values.size()));
    }, [&] {
        RMQ table(values, CapturedCmp{}); return std::uint64_t(table(0, values.size()));
    });
}
