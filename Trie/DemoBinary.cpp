#include "Final.hpp"

#include <iostream>
#include <optional>

struct BinaryInfo { int count = 0; };
constexpr int Bits = 30;
using Tree = BinaryTrie<BinaryInfo, unsigned, Bits>;

std::optional<unsigned> maxXor(const Tree& tree, unsigned x) {
    std::optional<unsigned> res;
    tree.walk([&](const Tree::Node& u, int dep) {
        if (u.info.count == 0 || dep == Bits) return -1;
        if (dep == 0) res = 0;
        const int bit = Bits - 1 - dep, b = (x >> bit) & 1;
        int d = b ^ 1;
        if (u[d].info.count == 0) d = b;
        *res |= unsigned(d ^ b) << bit;
        return d;
    });
    return res;
}

int main() {
    Tree::clearInit();
    Tree tree;
    for (const auto val : {2u, 7u, 9u, 12u}) tree.modify(val, [](BinaryInfo& info, int) { ++info.count; });
    const unsigned x = 6;
    const auto ans = maxXor(tree, x);
    if (ans) std::cout << "key=" << (*ans ^ x) << ", max xor=" << *ans << '\n';
    else std::cout << "none\n";
    return 0;
}
