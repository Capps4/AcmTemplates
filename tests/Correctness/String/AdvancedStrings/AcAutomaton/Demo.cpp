#include "../../../../../src/String/AdvancedStrings/AcAutomaton/code.hpp"

int main() {
    // Demo 1：完整模式只在末字符登记，空模式通过返回的根单独登记。
    std::array<std::string_view, 6> ps{"he", "she", "his", "hers", "he", ""};
    AcAutomaton<> ac;
    std::vector<int> endpos;
    std::vector<long long> end(1);
    for (auto s : ps) {
        int p = ac.add(s, [&](int p, int i) {
            end.resize(ac.son.size());
            if (i + 1 == int(s.size()))
                ++end[p];
        });
        if (s.empty())
            ++end[p];
        endpos.push_back(p);
    }
    ac.build();
    std::vector<long long> cnt(ac.son.size());
    cnt[0] = 1; // 空模式匹配起始边界，随后每个字符再贡献一个边界。
    int p = 0;
    for (char c : std::string_view("ushers")) {
        p = ac.step(p, c);
        ++cnt[p];
    }
    for (int i = int(ac.order.size()) - 1; i > 0; --i) {
        int x = ac.order[i];
        cnt[ac.link[x]] += cnt[x];
    }
    std::array<long long, 6> want{1, 1, 0, 1, 1, 7};
    for (int i = 0; i < int(ps.size()); ++i) {
        assert(cnt[endpos[i]] == want[i]);
        std::cout << '"' << ps[i] << "\": " << cnt[endpos[i]] << '\n';
    }

    // 沿 link 正序传播终点权重，保留原始 end 以便重复计算。
    auto hit = end;
    for (int i = 1; i < int(ac.order.size()); ++i) {
        int x = ac.order[i];
        hit[x] += hit[ac.link[x]];
    }
    long long sum = hit[0];
    p = 0;
    for (char c : std::string_view("ushers")) {
        p = ac.step(p, c);
        sum += hit[p];
    }
    assert(sum == 11 and end[0] == 1);
    ac.build();
    assert(ac.order.size() == ac.son.size());
    std::cout << "total: " << sum << '\n';

    // Demo 2：多文本共用访问次数数组，只汇总一次；每串从根重新扫描。
    std::vector<long long> bag(ac.son.size());
    for (auto s : {"ushers", "she", ""}) {
        p = 0;
        ++bag[0];
        for (char c : std::string_view(s)) {
            p = ac.step(p, c);
            ++bag[p];
        }
    }
    for (int i = int(ac.order.size()) - 1; i > 0; --i) {
        int x = ac.order[i];
        bag[ac.link[x]] += bag[x];
    }
    want = {2, 2, 0, 1, 2, 12};
    for (int i = 0; i < int(ps.size()); ++i)
        assert(bag[endpos[i]] == want[i]);
    std::cout << "batch he: " << bag[endpos[0]] << '\n';

    // Demo 3：禁用标记也只属于题目代码。
    AcAutomaton<2> ban;
    std::vector<bool> bad(1);
    for (auto s : {std::string_view("aab"), std::string_view("bb")})
        ban.add(s, [&](int p, int i) {
            bad.resize(ban.son.size());
            if (i + 1 == int(s.size()))
                bad[p] = true;
        });
    ban.build();
    for (int i = 1; i < int(ban.order.size()); ++i) {
        int x = ban.order[i];
        bad[x] = bad[x] or bad[ban.link[x]];
    }
    auto safe = [&](std::string_view s) {
        int x = 0;
        if (bad[x])
            return false;
        for (char c : s) {
            x = ban.step(x, c);
            if (bad[x])
                return false;
        }
        return true;
    };
    assert(safe("aba") and not safe("aab") and not safe("abb"));
    std::cout << std::boolalpha << "safe aba: " << safe("aba") << '\n';
    std::cout << "safe abb: " << safe("abb") << '\n';
    return 0;
}
