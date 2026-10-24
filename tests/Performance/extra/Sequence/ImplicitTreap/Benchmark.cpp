#include "../../../../../src/extra/Sequence/ImplicitTreap/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

struct Hash {
    std::uint64_t value, power;
    explicit Hash(std::uint64_t value, std::uint64_t power = 1000003)
        : value(value), power(power) {}
    Hash operator+(const Hash& other) const {
        return Hash(value * other.power + other.value, power * other.power);
    }
};

struct Operation {
    int type, l, r, ql, qr;
    std::uint64_t value, expected;
};

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<Hash> initial;
    std::vector<std::uint64_t> plain;
    for (int i = 0; i < in.n; ++i) {
        auto value = rng() % 1000;
        initial.emplace_back(value);
        plain.push_back(value);
    }
    std::vector<Operation> ops;
    for (int step = 0; step < std::min(in.n, 2000); ++step) {
        int type = in.shape == 0 ? 0 : step % 3;
        int n = int(plain.size()), l = int(rng() % n), r = l + 1 + int(rng() % (n - l));
        auto value = rng() % 1000;
        if (type == 0) {
            std::reverse(plain.begin() + l, plain.begin() + r);
        } else if (type == 1) {
            plain.insert(plain.begin() + l, value);
        } else {
            r = l + 1;
            plain.erase(plain.begin() + l);
        }
        n = int(plain.size());
        int ql = int(rng() % n), qr = ql + 1 + int(rng() % (n - ql));
        std::uint64_t expected = 0;
        for (int i = ql; i < qr; ++i) {
            expected = expected * 1000003 + plain[i];
        }
        ops.push_back({type, l, r, ql, qr, value, expected});
    }
    return measure(in, "reverse / insert erase sequence with directional hash oracle", [&]() {
        ImplicitTreap<Hash> tree(initial, in.seed);
        std::uint64_t checksum = 0;
        for (const auto& op : ops) {
            if (op.type == 0) {
                tree.reverse(op.l, op.r);
            } else if (op.type == 1) {
                tree.insert(op.l, Hash(op.value));
            } else {
                tree.erase(op.l, op.r);
            }
            auto result = tree.query(op.ql, op.qr).value;
            benchmarkCheck(result == op.expected, "vector directional polynomial hash");
            checksum = benchmarkMix(checksum, result);
        }
        return checksum;
    });
}
