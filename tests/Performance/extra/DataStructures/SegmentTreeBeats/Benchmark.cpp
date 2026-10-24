#include "../../../../../src/extra/DataStructures/SegmentTreeBeats/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

struct Operation {
    int type, l, r, ql, qr;
    long long value, sum, low, high;
};

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<long long> initial(in.n);
    for (auto& value : initial) {
        value = static_cast<long long>(rng() % 20001) - 10000;
    }
    auto plain = initial;
    std::vector<Operation> ops;
    for (int step = 0; step < std::min(in.n, 2000); ++step) {
        int l = int(rng() % in.n), r = int(rng() % in.n);
        if (l > r) {
            std::swap(l, r);
        }
        r = in.shape == 0 ? r + 1 : std::min(in.n, l + 1 + int(rng() % 32));
        int type = step % 3;
        long long value = static_cast<long long>(rng() % 1001) - 500;
        for (int i = l; i < r; ++i) {
            if (type == 0) {
                plain[i] = std::min(plain[i], value);
            } else if (type == 1) {
                plain[i] = std::max(plain[i], value);
            } else {
                plain[i] += value;
            }
        }
        int ql = int(rng() % in.n), qr = ql + 1 + int(rng() % (in.n - ql));
        long long sum = 0, low = plain[ql], high = plain[ql];
        for (int i = ql; i < qr; ++i) {
            sum += plain[i];
            low = std::min(low, plain[i]);
            high = std::max(high, plain[i]);
        }
        ops.push_back({type, l, r, ql, qr, value, sum, low, high});
    }
    return measure(in, "broad / short clamps and adds with independent range oracle", [&]() {
        SegmentTreeBeats tree(initial);
        std::uint64_t checksum = 0;
        for (const auto& op : ops) {
            if (op.type == 0) {
                tree.chmin(op.l, op.r, op.value);
            } else if (op.type == 1) {
                tree.chmax(op.l, op.r, op.value);
            } else {
                tree.add(op.l, op.r, op.value);
            }
            auto result = tree.query(op.ql, op.qr);
            benchmarkCheck(result.sum == op.sum and result.mn == op.low and result.mx == op.high,
                           "naive sum and extrema");
            checksum = benchmarkMix(checksum, result.sum);
            checksum = benchmarkMix(checksum, result.mn);
            checksum = benchmarkMix(checksum, result.mx);
        }
        return checksum;
    });
}
