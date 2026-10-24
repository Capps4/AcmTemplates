#include "Input.hpp"

// Nyaan's CC0 period-bucket implementation, retaining its independent filtering.
// https://nyaannyaan.github.io/library/string/run-enumerate.hpp.html
std::vector<std::tuple<int, int, int>> reference(std::string_view s) {
    int n = s.size();
    std::vector<std::vector<std::pair<int, int>>> buckets(n + 1);
    auto cross = [&](std::string a, const std::string &b, int mid, bool rev) {
        int l = a.size(), r = b.size();
        std::string joined = b + a + b;
        std::reverse(a.begin(), a.end());
        auto x = zFunction(a), y = zFunction(joined);
        for (int p = 1; p <= l; ++p) {
            int u = p == l ? p : std::min(x[p] + p, l);
            int v = std::min(y[l + r - p], r);
            if (u + v >= 2 * p)
                buckets[p].emplace_back(mid - (rev ? v : u), mid + (rev ? u : v));
        }
    };
    auto dfs = [&](auto &self, int l, int r) -> void {
        if (r - l <= 1)
            return;
        int mid = (l + r) / 2;
        self(self, l, mid);
        self(self, mid, r);
        std::string a(s.substr(l, mid - l)), b(s.substr(mid, r - mid));
        cross(a, b, mid, false);
        std::reverse(a.begin(), a.end());
        std::reverse(b.begin(), b.end());
        cross(b, a, mid, true);
    };
    dfs(dfs, 0, n);
    std::vector<std::tuple<int, int, int>> result;
    std::set<std::pair<int, int>> seen;
    for (int p = 1; p <= n; ++p) {
        auto &ranges = buckets[p];
        std::sort(ranges.begin(), ranges.end(), [](auto x, auto y) {
            return x.first != y.first ? x.first < y.first : x.second > y.second;
        });
        int right = -1;
        for (auto range : ranges) {
            if (right >= range.second)
                continue;
            right = range.second;
            if (seen.insert(range).second)
                result.emplace_back(p, range.first, range.second);
        }
    }
    return result;
}

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    auto s = workload(input);
    benchmarkCheck(reference(s) == runEnumeration(s), "complete source-reference comparison");
    return measure(input, "Nyaan period buckets: random 4-letter / Fibonacci word", [&] {
        return checksum(reference(s), s);
    });
}
