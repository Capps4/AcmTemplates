#include "../../../../../src/extra/DataStructures/MatBIT/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

struct Point {
    int x, y;
    long long value;
};

struct Rectangle {
    int u, l, d, r;
    long long value;
};

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    int n = in.n;
    std::vector<std::vector<Point>> updates(2);
    std::vector<std::vector<Rectangle>> queries(2);
    std::vector<std::vector<long long>> plain(n, std::vector<long long>(n));
    for (int phase = 0; phase < 2; ++phase) {
        for (int i = 0; i < 2 * n; ++i) {
            int span = in.shape ? std::max(1, n / 8) : n;
            int offset = in.shape ? n / 3 : 0;
            int x = offset + int(rng() % span), y = offset + int(rng() % span);
            long long value = static_cast<long long>(rng() % 21) - 10;
            updates[phase].push_back({x, y, value});
            plain[x][y] += value;
        }
        std::vector<std::vector<long long>> prefix(n + 1, std::vector<long long>(n + 1));
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < n; ++y) {
                prefix[x + 1][y + 1] = plain[x][y] + prefix[x][y + 1]
                    + prefix[x + 1][y] - prefix[x][y];
            }
        }
        for (int i = 0; i < 2 * n; ++i) {
            int u = int(rng() % n), l = int(rng() % n);
            int d = u + 1 + int(rng() % (n - u)), r = l + 1 + int(rng() % (n - l));
            if (in.shape and i % 2 == 0) {
                u = l = n / 3;
                d = r = std::min(n, n / 3 + std::max(1, n / 8));
            }
            if (i % 16 == 0) {
                u = l = 0;
                d = r = n;
            } else if (i % 16 == 1) {
                d = u;
            }
            queries[phase].push_back({u, l, d, r,
                prefix[d][r] - prefix[u][r] - prefix[d][l] + prefix[u][l]});
        }
    }
    return measure(in, "spread / clustered points with intermediate rectangle queries", [&]() {
        MatBIT<long long> tree(n, n);
        std::uint64_t checksum = 0;
        for (int phase = 0; phase < 2; ++phase) {
            for (auto update : updates[phase]) {
                tree.modify(update.x, update.y, update.value);
            }
            for (auto query : queries[phase]) {
                auto value = tree.rangeQuery(query.u, query.l, query.d, query.r);
                benchmarkCheck(value == query.value, "ordinary matrix checkpoint sum");
                checksum = benchmarkMix(checksum, value);
            }
        }
        return checksum;
    });
}
