#include "../../../../Support/CaseSupport.hpp"
// 复用 demo 的查询函数，避免测试另写一份同名算法。
#include "../../../../../src/DataStructures/TreeDataStructures/Trie/code.hpp"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <optional>
#include <random>
#include <string>
#include <vector>

namespace strings {
#define main stringDemoMain
#include "DemoString.cpp"
#undef main
}
namespace binary {
#define main binaryDemoMain
#include "DemoBinary.cpp"
#undef main
}
namespace paths {
#define main pathDemoMain
#include "Demo.cpp"
#undef main
}

std::mt19937 rng(20261004);

void testStrings() {
    using Tree = strings::Tree;
    Tree::clearInit(); // 不预留，覆盖 vector 扩容。
    std::array<Tree, 12> trees;
    std::array<std::map<std::string, int>, 12> models;
    const std::vector<std::string> keys{"", "a", "ab", "abc", "ac", "b", "ba", "c", "cc", "cccc", "z"};
    for (int step = 0; step < 48; ++step) {
        const int i = rng() % trees.size(), j = rng() % trees.size();
        switch (rng() % 5) {
        case 0:
            trees[i] = trees[j]; models[i] = models[j]; break;
        case 1: {
            Tree cp(trees[j]); trees[i] = std::move(cp); models[i] = models[j];
            assert(cp.query("").pass == 0);
            break;
        }
        case 2: {
            if (i == j) break;
            trees[i] = std::move(trees[j]); models[i] = models[j]; models[j].clear();
            assert(trees[j].query("").pass == 0);
            break;
        }
        default: {
            const auto& key = keys[rng() % keys.size()];
            const int delta = models[i][key] > 0 && rng() % 2 ? -1 : 1;
            models[i][key] += delta;
            int visits = 0;
            trees[i].modify(key, [&](strings::StringInfo& info, int dep) {
                assert(dep == visits++);
                info.pass += delta;
                if (dep == int(key.size())) info.end += delta;
            });
            assert(visits == int(key.size()) + 1);
        }
        }
        for (std::size_t t = 0; t < trees.size(); ++t) {
            for (const auto& key : keys) {
                int pass = 0, end = 0;
                for (const auto& [s, n] : models[t]) {
                    if (s.compare(0, key.size(), key) == 0) pass += n;
                    if (s == key) end += n;
                }
                const auto got = trees[t].query(key);
                assert(got.pass == pass && got.end == end);
                std::optional<std::string> expected;
                for (auto it = models[t].lower_bound(key); it != models[t].end(); ++it) {
                    if (it->second > 0) { expected = it->first; break; }
                }
                assert(strings::lowerBound(trees[t], key) == expected);
            }
            assert(trees[t].query("zzzz").pass == 0);
        }
    }
    // 高字节编码不受 char 是否有符号影响。
    using Bytes = StringTrie<strings::StringInfo, 256, '\0'>;
    Bytes::clearInit(); Bytes bytes;
    const std::string key{char(0), char(128), char(255)};
    bytes.modify(key, [](auto& info, int) { ++info.pass; });
    assert(bytes.query(key).pass == 1);
    // 核心迭代遍历长串；不调用 demo 中的递归 DFS。
    Tree::clearInit(); Tree longTree;
    const std::string longKey(20000, 'a');
    longTree.modify(longKey, [](auto& info, int) { ++info.pass; });
    assert(longTree.query(longKey).pass == 1);
}

void testBinary() {
    using Tree = binary::Tree;
    Tree::clearInit();
    std::array<Tree, 12> trees;
    std::array<std::array<int, 64>, 12> models{};
    for (int step = 0; step < 48; ++step) {
        const int i = rng() % trees.size(), j = rng() % trees.size();
        switch (rng() % 5) {
        case 0: trees[i] = trees[j]; models[i] = models[j]; break;
        case 1: {
            Tree cp(trees[j]); trees[i] = std::move(cp); models[i] = models[j];
            assert(!binary::maxXor(cp, 0));
            break;
        }
        case 2: {
            if (i == j) break;
            trees[i] = std::move(trees[j]); models[i] = models[j]; models[j].fill(0);
            assert(!binary::maxXor(trees[j], 0));
            break;
        }
        default: {
            const unsigned x = rng() % 64;
            const int delta = models[i][x] > 0 && rng() % 2 ? -1 : 1;
            models[i][x] += delta;
            trees[i].modify(x, [&](auto& info, int) { info.count += delta; });
        }
        }
        for (std::size_t t = 0; t < trees.size(); ++t) {
            for (unsigned x = 0; x < 64; ++x) {
                assert(trees[t].query(x).count == models[t][x]);
                std::optional<unsigned> expected;
                for (unsigned v = 0; v < 64; ++v) {
                    if (models[t][v] > 0 && (!expected || (v ^ x) > *expected)) expected = v ^ x;
                }
                assert(binary::maxXor(trees[t], x) == expected);
            }
        }
    }
}

void testPaths() {
    using Tree = paths::Tree;
    Tree::clearInit();
    constexpr int N = 80;
    std::array<Tree, N + 1> prefix, version;
    std::array<unsigned, N + 1> values{};
    std::array<int, N + 1> parent{}, depth{};
    for (int i = 1; i <= N; ++i) {
        values[i] = rng() % 16;
        prefix[i] = prefix[i - 1];
        prefix[i].modify(values[i], [](auto& info, int) { ++info.count; });
        parent[i] = rng() % i; depth[i] = depth[parent[i]] + 1;
        version[i] = version[parent[i]];
        version[i].modify(values[i], [](auto& info, int) { ++info.count; });
    }
    for (int trial = 0; trial < 16; ++trial) {
        int l = 1 + rng() % N, r = 1 + rng() % N;
        if (l > r) std::swap(l, r);
        std::vector<unsigned> sorted(values.begin() + l, values.begin() + r + 1);
        std::sort(sorted.begin(), sorted.end());
        const int k = 1 + rng() % sorted.size();
        assert(paths::kth(prefix[r], prefix[l - 1], k) == sorted[k - 1]);

        int u = 1 + rng() % N, v = 1 + rng() % N, a = u, b = v;
        sorted.clear();
        while (a != b) {
            if (depth[a] >= depth[b]) { sorted.push_back(values[a]); a = parent[a]; }
            else { sorted.push_back(values[b]); b = parent[b]; }
        }
        // 虚拟 0 号根没有点权；不同分支在 0 相交时，减两个空版本。
        if (a) sorted.push_back(values[a]);
        std::sort(sorted.begin(), sorted.end());
        const int kth = 1 + rng() % sorted.size();
        assert(paths::kth(version[u], version[v], version[a], version[parent[a]], kth) == sorted[kth - 1]);
    }
}

void testBoundaries() {
    struct TestInfo { int count = 0; };
    using Tree = BinaryTrie<TestInfo, std::uint64_t>;
    static_assert(std::numeric_limits<std::uint32_t>::digits == 32);
    static_assert(std::numeric_limits<std::uint64_t>::digits == 64);
    static_assert(std::numeric_limits<std::uint64_t>::radix == 2);
    static_assert(std::is_same_v<decltype(std::declval<const Tree::Node&>()[0]), const Tree::Node&>);
    static_assert(sizeof(Tree) == 2 * sizeof(int));
    Tree::clearInit(); Tree tree, empty;
    auto add = [](TestInfo& info, int) { ++info.count; };
    const std::array<std::uint64_t, 4> keys{0, 1, std::uint64_t{1} << 63, UINT64_MAX};
    for (const auto x : keys) tree.modify(x, add);
    for (const auto x : keys) assert(tree.query(x).count == 1);
    assert(tree.query(2).count == 0);
    // 单根 walk 全长 64，两个空节点同路遍历也不提前停止。
    int visits = 0;
    empty.walk(empty, [&](const auto& a, const auto& b, int dep) {
        assert(a.info.count == 0 && b.info.count == 0 && a[1].info.count == 0);
        assert(dep == visits++);
        return dep == 64 ? -1 : 1;
    });
    assert(visits == 65);
    for (const auto x : keys) {
        std::uint64_t result = 0;
        tree.walk([&](const auto& u, int dep) {
            if (dep == 64) return -1;
            const int bit = 63 - dep, b = (x >> bit) & 1;
            const int d = u[b ^ 1].info.count > 0 ? b ^ 1 : b;
            result |= std::uint64_t(d ^ b) << bit;
            return d;
        });
        std::uint64_t expected = 0;
        for (const auto y : keys) expected = std::max(expected, x ^ y);
        assert(result == expected);
    }
    tree.modify(0, add); // 唯一路径更新。
    assert(tree.query(0).count == 2);
    auto old = tree;
    tree.modify(0, add); // 复制后修改，旧版本保持不变。
    assert(old.query(0).count == 2 && tree.query(0).count == 3);
    old.modify(UINT64_MAX, add); // 源版本也写，互不影响。
    assert(old.query(UINT64_MAX).count == 2 && tree.query(UINT64_MAX).count == 1);
    Tree* self = &tree;
    tree = *self; tree = std::move(*self);
    assert(tree.query(0).count == 3);
    Tree moved(std::move(tree));
    assert(moved.query(0).count == 3 && tree.query(0).count == 0);
    tree.modify(1, add); // 移动后的空树可重新使用。
    assert(tree.query(1).count == 1 && moved.query(1).count == 1);
    using Full32 = BinaryTrie<TestInfo, std::uint32_t>;
    Full32::clearInit(); Full32 full32;
    full32.modify(std::uint32_t{1} << 31, add).modify(UINT32_MAX, add);
    assert(full32.query(std::uint32_t{1} << 31).count == 1 && full32.query(UINT32_MAX).count == 1);
    int full32Visits = 0;
    full32.walk([&](const auto& u, int dep) {
        assert(dep == full32Visits++);
        if (dep == 32) { assert(u.info.count == 1); return -1; }
        return 1;
    });
    assert(full32Visits == 33);
    using OneBit = BinaryTrie<TestInfo, unsigned char, 1>;
    OneBit::clearInit(); OneBit one;
    one.modify(0, add).modify(1, add);
    assert(one.query(0).count == 1 && one.query(1).count == 1);
    // 新一轮初始化；不再访问前一轮的旧树。
    Tree::clearInit(); Tree reset;
    assert(reset.query(UINT64_MAX).count == 0);
}



#include "../../../../Support/CaseSupport.hpp"

namespace boundary_cases {
struct AddedInfo {
    int pass = 0, end = 0;
};
using AddedTrie = StringTrie<AddedInfo>;
void verifyAdded(const std::vector<std::string> &words) {
    AddedTrie::clearInit();
    AddedTrie d;
    auto verify = [&](const AddedTrie &t, const std::vector<std::string> &ws) {
        std::set<std::string> queries{"", "a", "z", "missing"};
        for (auto &s : ws)
            for (std::size_t k = 0; k <= s.size(); ++k)
                queries.insert(s.substr(0, k));
        for (auto &q : queries) {
            int pass = 0, end = 0;
            for (auto &s : ws) {
                pass += s.substr(0, q.size()) == q;
                end += s == q;
            }
            auto got = t.query(q);
            CHECK(got.pass == pass);
            CHECK(got.end == end);
        }
    };
    std::vector<std::string> seen;
    for (auto &s : words) {
        auto old = d;
        auto snapshot = seen;
        d.modify(s, [&](AddedInfo &i, int dep) {
            ++i.pass;
            if (dep == int(s.size()))
                ++i.end;
        });
        seen.push_back(s);
        verify(d, seen);
        verify(old, snapshot);
    }
    verify(d, seen);
}

int run() {
    runCase("Trie/empty", [] {
        verifyAdded({});
    });
    runCase("Trie/empty-key", [] {
        verifyAdded({""});
    });
    runCase("Trie/single-key", [] {
        verifyAdded({"a"});
    });
    runCase("Trie/duplicate-prefix", [] {
        verifyAdded({"a", "a", "aa"});
    });
    runCase("Trie/branching-prefixes", [] {
        verifyAdded({"ab", "abc", "abd"});
    });
    runCase("Trie/deep-shared-prefix", [] {
        verifyAdded({"z", "zz", "zzz"});
    });
    runCase("Trie/disjoint-branches", [] {
        verifyAdded({"abc", "bca", "cab"});
    });
    runCase("Trie/duplicate-empty-key", [] {
        verifyAdded({"", "a", ""});
    });
    runCase("Trie/overlapping-words", [] {
        verifyAdded({"ababa", "aba", "ba"});
    });
    runCase("Trie/mixed-word-lengths", [] {
        verifyAdded({"apple", "app", "bat", "cat"});
    });
    return 0;
}
}


int main() {
    runCase("Trie/testStrings", [] { testStrings(); });
    runCase("Trie/testBinary", [] { testBinary(); });
    runCase("Trie/testPaths", [] { testPaths(); });
    runCase("Trie/testBoundaries", [] { testBoundaries(); });
    CHECK(boundary_cases::run() == 0);
    return 0;
}
