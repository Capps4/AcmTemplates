#include "../../../../../src/String/AdvancedStrings/ExSuffixAutomaton/code.hpp"

int main() {
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
