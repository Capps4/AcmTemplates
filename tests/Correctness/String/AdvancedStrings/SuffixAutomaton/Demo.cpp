#include "../../../../../src/String/AdvancedStrings/SuffixAutomaton/code.hpp"

int main() {
    // Demo 1：次数、代表位置及每个前缀的状态均由外部数组保存。
    std::string s = "banana";
    Sam<> sam(int(s.size()));
    std::vector<long long> cnt(1);
    std::vector<int> pos(1, -1), endpos(s.size());
    sam.add(s, [&](int p, int i) {
        cnt.resize(sam.len.size()); // 同时给本次产生的 clone 补 0。
        pos.resize(sam.len.size(), -1);
        ++cnt[p];
        pos[p] = i;
        endpos[i] = p;
    });
    auto res = cnt;
    auto loc = pos;
    auto ord = sam.getOrder();
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int p = ord[i], f = sam.link[p];
        res[f] += res[p];
        if (loc[f] < 0)
            loc[f] = loc[p];
    }
    long long num = 0;
    for (int p = 1; p < int(sam.len.size()); ++p)
        num += sam.len[p] - sam.len[sam.link[p]];
    assert(num == 15 and endpos.back() == sam.last);
    std::cout << "distinct: " << num << '\n';
    for (auto t : {std::string_view("a"), std::string_view("ana")}) {
        auto p = sam.find(t);
        assert(p and *p > 0);
        int l = loc[*p] - int(t.size()) + 1;
        assert(std::string_view(s).substr(l, t.size()) == t);
        std::cout << t << ": count=" << res[*p] << ", at=" << l << '\n';
    }
    int a = *sam.find("a"), ana = *sam.find("ana");
    assert(res[a] == 3 and res[ana] == 2);
    assert(not sam.find("apple"));
    auto root = sam.find("");
    assert(root and *root == 0);

    // Demo 2：另一文本的最长公共子串。
    int p = 0, l = 0, lcs = 0;
    for (char c : std::string_view("canada")) {
        sam.match(p, l, c);
        lcs = std::max(lcs, l);
        std::cout << l << ' ';
    }
    std::cout << '\n';
    assert(lcs == 3);
    std::cout << "lcs canada: " << lcs << '\n';

    // Demo 3：多串 LCS 的传播须截断到 link 父亲的长度。
    auto best = sam.len;
    for (auto t : {"ananas", "canada"}) {
        std::vector<int> mx(sam.len.size());
        p = 0;
        l = 0;
        for (char c : std::string_view(t)) {
            sam.match(p, l, c);
            mx[p] = std::max(mx[p], l);
        }
        for (int i = int(ord.size()) - 1; i > 0; --i) {
            int x = ord[i], f = sam.link[x];
            mx[f] = std::max(mx[f], std::min(mx[x], sam.len[f]));
        }
        for (int x = 0; x < int(sam.len.size()); ++x)
            best[x] = std::min(best[x], mx[x]);
    }
    int ans = *std::max_element(best.begin(), best.end());
    assert(ans == 3);
    std::cout << "lcs of three: " << ans << '\n';

    // 单字符追加直接返回节点，重新从直接贡献汇总，旧 res 不变。
    p = sam.add('a');
    cnt.resize(sam.len.size());
    pos.resize(sam.len.size(), -1);
    ++cnt[p];
    pos[p] = sam.len[p] - 1;
    auto now = cnt;
    ord = sam.getOrder();
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int x = ord[i];
        now[sam.link[x]] += now[x];
    }
    assert(res[a] == 3 and now[*sam.find("a")] == 4);
    num = 0;
    for (int x = 1; x < int(sam.len.size()); ++x)
        num += sam.len[x] - sam.len[sam.link[x]];
    assert(num == 21);
    std::cout << "distinct after append: " << num << '\n';

    // Demo 4：abb 中 b 的节点是 clone，零贡献经 link 汇总后得到两次出现。
    Sam<2> tmp;
    std::vector<long long> raw(1);
    std::array<int, 3> ends{};
    tmp.add("abb", [&](int x, int i) {
        raw.resize(tmp.len.size());
        ++raw[x];
        ends[i] = x;
    });
    int b = *tmp.find("b");
    assert(raw[b] == 0 and ends[2] < b);
    auto all = raw;
    auto seq = tmp.getOrder();
    for (int i = int(seq.size()) - 1; i > 0; --i) {
        int x = seq[i];
        all[tmp.link[x]] += all[x];
    }
    assert(all[b] == 2 and all[*tmp.find("ab")] == 1);
    std::cout << "clone b: direct=" << raw[b] << ", count=" << all[b] << '\n';
    return 0;
}
