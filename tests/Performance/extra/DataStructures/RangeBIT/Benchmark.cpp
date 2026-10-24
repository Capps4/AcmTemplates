#include "../../../../../src/extra/DataStructures/RangeBIT/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

struct Range {
    int l, r;
    long long value;
};

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<std::vector<Range>> updates(2), queries(2);
    std::vector<long long> difference(in.n + 1);
    auto interval = [&]() {
        int l = int(rng() % in.n);
        int width = in.shape ? std::min(16, in.n - l) : in.n - l;
        return std::pair{l, l + 1 + int(rng() % width)};
    };
    for (int phase = 0; phase < 2; ++phase) {
        for (int i = 0; i < in.n; ++i) {
            auto [l, r] = interval();
            long long value = static_cast<long long>(rng() % 21) - 10;
            updates[phase].push_back({l, r, value});
            difference[l] += value;
            difference[r] -= value;
        }
        // An ordinary difference array gives every expected sum at this checkpoint.
        std::vector<long long> prefix(in.n + 1);
        long long value = 0;
        for (int i = 0; i < in.n; ++i) {
            value += difference[i];
            prefix[i + 1] = prefix[i] + value;
        }
        for (int i = 0; i < in.n; ++i) {
            auto [l, r] = interval();
            if (i % 16 == 0) {
                l = 0;
                r = in.n;
            } else if (i % 16 == 1) {
                r = l;
            }
            queries[phase].push_back({l, r, prefix[r] - prefix[l]});
        }
    }
    return measure(in, "wide / short range updates with intermediate subrange queries", [&]() {
        RangeBIT<long long> tree(in.n);
        std::uint64_t checksum = 0;
        for (int phase = 0; phase < 2; ++phase) {
            for (auto update : updates[phase]) {
                tree.rangeModify(update.l, update.r, update.value);
            }
            for (auto query : queries[phase]) {
                auto value = tree.rangeQuery(query.l, query.r);
                benchmarkCheck(value == query.value, "difference-array checkpoint sum");
                checksum = benchmarkMix(checksum, value);
            }
        }
        return checksum;
    });
}
