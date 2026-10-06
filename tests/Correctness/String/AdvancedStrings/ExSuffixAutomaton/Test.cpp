#include "../../../../Support/CaseSupport.hpp"
#include "../../../../Support/AutomataSupport.hpp"


int coreCases() {
    return automata_test::suite(2);
}

#include "../../../../Support/AutomataSupport.hpp"

namespace boundary_cases {

int run() {
    runCase("ExSuffixAutomaton/empty-source-set", [] {
        automata_test::exCase<26, 'a'>({});
    });
    runCase("ExSuffixAutomaton/empty-source", [] {
        automata_test::exCase<26, 'a'>({""});
    });
    runCase("ExSuffixAutomaton/duplicate-sources", [] {
        automata_test::exCase<26, 'a'>({"a", "a", "aa"});
    });
    runCase("ExSuffixAutomaton/shared-prefixes", [] {
        automata_test::exCase<26, 'a'>({"he", "she", "hers", "his"});
    });
    runCase("ExSuffixAutomaton/overlapping-sources", [] {
        automata_test::exCase<26, 'a'>({"ab", "bc", "abc"});
    });
    runCase("ExSuffixAutomaton/distinct-sources", [] {
        automata_test::exCase<26, 'a'>({"abc", "def"});
    });
    runCase("ExSuffixAutomaton/suffix-reuse", [] {
        automata_test::exCase<26, 'a'>({"a", "ab", "abc", "abcd"});
    });
    runCase("ExSuffixAutomaton/empty-duplicates", [] {
        automata_test::exCase<26, 'a'>({"", "", "aba"});
    });
    runCase("ExSuffixAutomaton/disjoint-alphabets", [] {
        automata_test::exCase<26, 'a'>({"zzzz", "x"});
    });
    runCase("ExSuffixAutomaton/periodic-sources", [] {
        automata_test::exCase<26, 'a'>({"abcabc", "bcabc", "cabc"});
    });
    return 0;
}
}

#include "../../../../../src/String/AdvancedStrings/ExSuffixAutomaton/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>

namespace callback_cases {

int run() {
    ExSam<128, char(128)> a(0);
    int calls = 0;
    std::vector<std::pair<int, int>> seen;
    auto fn = [&, own = std::make_unique<int>(7)](int p, int i) {
        CHECK(*own == 7);
        seen.emplace_back(p, i);
        ++calls;
    };
    static_assert(not std::is_copy_constructible_v<decltype(fn)>);
    std::string s("\x80\xff\xff\x80", 4);
    a.add("", fn);
    CHECK(calls == 0);
    a.add(s, fn);
    CHECK(calls == 4);
    for (int i = 0; i < 4; ++i) {
        CHECK(seen[i].second == i);
        CHECK(seen[i].first > 0);
    }
    a.add(s, fn); // Reuse the same noncopyable lvalue callback; i resets to zero.
    CHECK(calls == 8);
    for (int i = 0; i < 4; ++i)
        CHECK(seen[i + 4].second == i);
    CHECK(a.find("") == std::optional<int>(0));
    CHECK(a.find(s).has_value());
    CHECK(not a.find(std::string(1, char(129))));
    return 0;
}
}

#include "../../../../../src/String/AdvancedStrings/ExSuffixAutomaton/code.hpp"

namespace example_cases {

int run() {
    // Demo 1：来源编号由闭包捕获；重复内容的不同来源分别登记。
    std::array<std::string_view, 4> ss{"aba", "bab", "aba", "c"};
    ExSam<> sam;
    std::vector<long long> cnt(1);
    std::vector<unsigned> mask(1);
    for (int id = 0; id < int(ss.size()); ++id)
        sam.add(ss[id], [&](int p, int i) {
            assert(sam.len[p] == i + 1);
            cnt.resize(sam.len.size());
            mask.resize(sam.len.size());
            ++cnt[p];
            mask[p] |= 1U << id;
        });
    auto res = cnt;
    auto src = mask;
    auto ord = sam.getOrder();
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int p = ord[i], f = sam.link[p];
        res[f] += res[p];
        src[f] |= src[p];
    }
    int a = *sam.find("a"), aba = *sam.find("aba"), bab = *sam.find("bab");
    assert(res[a] == 5 and res[aba] == 2 and res[bab] == 1);
    assert(src[a] == 7 and src[aba] == 5 and src[bab] == 2);
    assert(not sam.find("ac"));
    auto root = sam.find("");
    assert(root and *root == 0);
    long long num = 0;
    for (int p = 1; p < int(sam.len.size()); ++p)
        num += sam.len[p] - sam.len[sam.link[p]];
    assert(num == 7);
    std::cout << "distinct: " << num << '\n';
    for (auto s : {"a", "aba", "bab", "c"}) {
        if (auto p = sam.find(s))
            std::cout << s << ": count=" << res[*p]
                      << ", sources=" << std::bitset<4>(src[*p]).count() << '\n';
    }

    // Demo 2：每个来源独有的不同子串数。
    std::vector<long long> ans(ss.size());
    for (int p = 1; p < int(sam.len.size()); ++p)
        for (int id = 0; id < int(ss.size()); ++id)
            if (src[p] == (1U << id))
                ans[id] += sam.len[p] - sam.len[sam.link[p]];
    assert((ans == std::vector<long long>{0, 1, 0, 1}));
    for (int id = 0; id < int(ss.size()); ++id)
        std::cout << id << ": unique=" << ans[id] << '\n';

    // Demo 3：查询串在整个词典中的最长匹配。
    int p = 0, l = 0, best = 0, i = 0;
    std::array<int, 5> want{1, 1, 2, 3, 1};
    for (char c : std::string_view("cabac")) {
        sam.match(p, l, c);
        assert(l == want[i]);
        ++i;
        best = std::max(best, l);
    }
    assert(best == 3);
    std::cout << "dictionary match: " << best << '\n';

    // 分支扩展只返回节点；实际前缀的贡献可在外部自行登记。
    ExSam<> tree;
    int x = tree.add(0, 'a');
    x = tree.add(x, 'b');
    tree.add(x, 'a');
    tree.add(x, 'b');
    assert(tree.find("aba") and tree.find("abb"));
    return 0;
}
}

int main() {
    runCase("ExSuffixAutomaton/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    runCase("ExSuffixAutomaton/callbacks-and-byte-rejection", [] { CHECK(callback_cases::run() == 0); });
    runCase("ExSuffixAutomaton/usage-example", [] { CHECK(example_cases::run() == 0); });
    return 0;
}
