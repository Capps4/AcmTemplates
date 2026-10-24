#include "../../../../../src/extra/DataStructures/RangeMatBIT/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

struct Rectangle {
    int u, l, d, r;
    long long value;
};

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    int n = in.n;
    std::vector<std::vector<Rectangle>> updates(2), queries(2);
    std::vector<std::vector<long long>> difference(n + 1, std::vector<long long>(n + 1));
    auto rectangle = [&]() {
        int u = int(rng() % n), l = int(rng() % n);
        int height = in.shape ? std::min(8, n - u) : n - u;
        int width = in.shape ? std::min(8, n - l) : n - l;
        return Rectangle{u, l, u + 1 + int(rng() % height), l + 1 + int(rng() % width), 0};
    };
    for (int phase = 0; phase < 2; ++phase) {
        for (int i = 0; i < 2 * n; ++i) {
            auto update = rectangle();
            update.value = static_cast<long long>(rng() % 21) - 10;
            updates[phase].push_back(update);
            difference[update.u][update.l] += update.value;
            difference[update.d][update.r] += update.value;
            difference[update.u][update.r] -= update.value;
            difference[update.d][update.l] -= update.value;
        }
        // Offline 2D differences reconstruct the actual cells, independently of Fenwick trees.
        auto plain = difference;
        std::vector<std::vector<long long>> prefix(n + 1, std::vector<long long>(n + 1));
        for (int x = 0; x < n; ++x) {
            for (int y = 0; y < n; ++y) {
                if (x > 0) {
                    plain[x][y] += plain[x - 1][y];
                }
                if (y > 0) {
                    plain[x][y] += plain[x][y - 1];
                }
                if (x > 0 and y > 0) {
                    plain[x][y] -= plain[x - 1][y - 1];
                }
                prefix[x + 1][y + 1] = plain[x][y] + prefix[x][y + 1]
                    + prefix[x + 1][y] - prefix[x][y];
            }
        }
        for (int i = 0; i < 2 * n; ++i) {
            auto query = rectangle();
            if (i % 16 == 0) {
                query.u = query.l = 0;
                query.d = query.r = n;
            } else if (i % 16 == 1) {
                query.r = query.l;
            }
            query.value = prefix[query.d][query.r] - prefix[query.u][query.r]
                - prefix[query.d][query.l] + prefix[query.u][query.l];
            queries[phase].push_back(query);
        }
    }
    return measure(in, "wide / small rectangle updates with intermediate offset queries", [&]() {
        RangeMatBIT<long long> tree(n, n);
        std::uint64_t checksum = 0;
        for (int phase = 0; phase < 2; ++phase) {
            for (auto update : updates[phase]) {
                tree.rangeModify(update.u, update.l, update.d, update.r, update.value);
            }
            for (auto query : queries[phase]) {
                auto value = tree.rangeQuery(query.u, query.l, query.d, query.r);
                benchmarkCheck(value == query.value, "offline matrix checkpoint sum");
                checksum = benchmarkMix(checksum, value);
            }
        }
        return checksum;
    });
}
