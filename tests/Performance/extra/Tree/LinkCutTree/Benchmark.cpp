#include "../../../../../src/extra/Tree/LinkCutTree/code.hpp"
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
    int vertex, parent, next, from, to;
    std::uint64_t value, expected;
};

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<Hash> initial;
    std::vector<std::uint64_t> values;
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < in.n; ++i) {
        auto value = rng() % 1000;
        initial.emplace_back(value);
        values.push_back(value);
        if (i > 0) {
            int p = in.shape == 0 ? i - 1 : 0;
            edges.push_back({i, p});
        }
    }
    std::vector<Operation> ops;
    // Move the final leaf: the initial path or star remains connected after each re-link.
    int leaf = in.n - 1, parent = in.n <= 1 ? -1 : in.shape == 0 ? in.n - 2 : 0;
    for (int step = 0; step < std::min(in.n, 2000); ++step) {
        int next = in.n <= 1 ? -1 : int(rng() % (in.n - 1));
        int vertex = int(rng() % in.n);
        auto value = rng() % 1000;
        values[vertex] = value;
        int from = step % 2 ? leaf : int(rng() % in.n);
        int to = step % 2 ? int(rng() % in.n) : leaf;
        std::uint64_t expected = 0;
        auto append = [&](int x) { expected = expected * 1000003 + values[x]; };
        // Removing the final leaf leaves an unchanged numbered path or a star at 0.
        // Generate that known path directly; the general BFS oracle lives in Correctness.
        if (from == to) {
            append(from);
        } else {
            int x = from == leaf ? next : from;
            int y = to == leaf ? next : to;
            if (from == leaf) {
                append(leaf);
            }
            if (in.shape == 0) {
                int direction = x <= y ? 1 : -1;
                for (int at = x;; at += direction) {
                    append(at);
                    if (at == y) {
                        break;
                    }
                }
            } else {
                append(x);
                if (x != y) {
                    if (x != 0 and y != 0) {
                        append(0);
                    }
                    append(y);
                }
            }
            if (to == leaf) {
                append(leaf);
            }
        }
        ops.push_back({vertex, parent, next, from, to, value, expected});
        parent = next;
    }
    return measure(in, "path / star leaf relinks with known directed paths", [&]() {
        LinkCutTree<Hash> tree(initial);
        for (auto [x, y] : edges) {
            benchmarkCheck(tree.link(x, y), "initial forest edges");
        }
        std::uint64_t checksum = 0;
        for (const auto& op : ops) {
            tree.modify(op.vertex, Hash(op.value));
            if (op.parent != -1) {
                benchmarkCheck(tree.cut(leaf, op.parent), "cut existing leaf edge");
                benchmarkCheck(not tree.connected(leaf, op.next), "detached leaf");
                benchmarkCheck(tree.link(leaf, op.next), "link detached leaf");
            }
            auto result = tree.query(op.from, op.to);
            benchmarkCheck(result and result->value == op.expected, "known path directed polynomial hash");
            checksum = benchmarkMix(checksum, result->value);
        }
        return checksum;
    });
}
