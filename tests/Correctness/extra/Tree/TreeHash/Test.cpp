#include "../../../../../src/extra/Tree/TreeHash/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

using Tree = std::vector<std::vector<int>>;

std::string canonical(const Tree& tree, int root) {
    std::vector<std::string> children;
    for (int child : tree[root]) {
        children.push_back(canonical(tree, child));
    }
    std::sort(children.begin(), children.end());
    std::string code = "(";
    for (const auto& child : children) {
        code += child;
    }
    return code + ")";
}

std::uint64_t hashTree(const Tree& tree, int root, const TreeHash& mix) {
    std::uint64_t result = 1;
    for (int child : tree[root]) {
        result += mix(hashTree(tree, child, mix));
    }
    return result;
}

int main() {
    runCase("TreeHash/oracle", [] {
        std::vector<std::uint64_t> seeds{0, 1, 42, ~std::uint64_t(0)};
        for (auto seed : seeds) {
            TreeHash a(seed), b(seed), c(seed + 1);
            CHECK(a(1) != a(std::uint64_t(1) << 31));
            std::set<std::uint64_t> inputs;
            inputs.insert(~std::uint64_t(0));
            inputs.insert(std::uint64_t(0) - seed - 0x9e3779b97f4a7c15ULL);
            for (int bit = 0; bit < 64; ++bit) {
                inputs.insert(std::uint64_t(1) << bit);
                inputs.insert(~(std::uint64_t(1) << bit));
            }
            for (std::uint64_t x = 0; x < 1024; ++x) {
                inputs.insert(x);
            }
            std::set<std::uint64_t> outputs;
            for (auto x : inputs) {
                CHECK(a(x) == b(x));
                CHECK(a(x) != c(x));
                CHECK(outputs.insert(a(x)).second);
            }
        }
        CHECK(f(123) == treeHash(123));

        // Add a leaf at every vertex; exact encodings deduplicate rooted shapes.
        // This enumerates every rooted tree up to eight vertices independently
        // of the integer hash, including chains, stars and repeated subtrees.
        std::map<std::string, Tree> shapes{{"()", Tree(1)}};
        std::vector<std::map<std::uint64_t, std::string>> hashes(seeds.size());
        const int counts[] = {1, 1, 2, 4, 9, 20, 48, 115};
        for (int n = 1; n <= 8; ++n) {
            CHECK(shapes.size() == std::size_t(counts[n - 1]));
            for (const auto& [code, tree] : shapes) {
                std::vector<int> permutation(n);
                std::iota(permutation.begin(), permutation.end(), 0);
                std::shuffle(permutation.begin(), permutation.end(), testRng);
                Tree renamed(n);
                for (int x = 0; x < n; ++x) {
                    for (int y : tree[x]) {
                        renamed[permutation[x]].push_back(permutation[y]);
                    }
                    std::shuffle(renamed[permutation[x]].begin(),
                                 renamed[permutation[x]].end(), testRng);
                }
                CHECK(canonical(renamed, permutation[0]) == code);
                for (std::size_t s = 0; s < seeds.size(); ++s) {
                    TreeHash mix(seeds[s]);
                    auto value = hashTree(tree, 0, mix);
                    CHECK(hashes[s].emplace(value, code).second);
                    CHECK(hashTree(renamed, permutation[0], mix) == value);
                }
            }
            if (n == 8) {
                break;
            }
            std::map<std::string, Tree> next;
            for (const auto& [code, tree] : shapes) {
                (void)code;
                for (int parent = 0; parent < n; ++parent) {
                    Tree grown = tree;
                    grown.emplace_back();
                    grown[parent].push_back(n);
                    auto key = canonical(grown, 0);
                    next.emplace(key, std::move(grown));
                }
            }
            shapes = std::move(next);
        }
    });
    return 0;
}
