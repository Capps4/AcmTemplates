#include "../../../../Support/CaseSupport.hpp"
#include "../../../../Support/AutomataSupport.hpp"


int coreCases() {
    return automata_test::suite(3);
}

#include "../../../../Support/AutomataSupport.hpp"

namespace boundary_cases {

int run() {
    runCase("PalindromeAutomaton/empty", [] {
        automata_test::pamCase<26, 'a'>("");
    });
    runCase("PalindromeAutomaton/single", [] {
        automata_test::pamCase<26, 'a'>("a");
    });
    runCase("PalindromeAutomaton/overlap", [] {
        automata_test::pamCase<26, 'a'>("aaabaaa");
    });
    runCase("PalindromeAutomaton/periodic-tail", [] {
        automata_test::pamCase<26, 'a'>("abcabcab");
    });
    runCase("PalindromeAutomaton/repeated-letters", [] {
        automata_test::pamCase<26, 'a'>("mississippi");
    });
    runCase("PalindromeAutomaton/alphabet", [] {
        automata_test::pamCase<26, 'a'>("abcdefghijklmnopqrstuvwxyz");
    });
    runCase("PalindromeAutomaton/nested-palindromes", [] {
        automata_test::pamCase<26, 'a'>("abacabadabacaba");
    });
    runCase("PalindromeAutomaton/skewed", [] {
        automata_test::pamCase<26, 'a'>("zzxyzzx");
    });
    runCase("PalindromeAutomaton/odd-palindrome", [] {
        automata_test::pamCase<26, 'a'>("abcdefedcba");
    });
    runCase("PalindromeAutomaton/even-palindromes", [] {
        automata_test::pamCase<26, 'a'>("abbaabba");
    });
    return 0;
}
}

#include "../../../../../src/String/AdvancedStrings/PalindromeAutomaton/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>

namespace callback_cases {

int run() {
    Pam<128, char(128)> a(0);
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
    CHECK(a.s == s + s);
    CHECK(a.len[0] == 0 and a.len[1] == -1);
    CHECK(a.link[0] == 1 and a.link[1] == 1);
    return 0;
}
}

#include "../../../../../src/String/AdvancedStrings/PalindromeAutomaton/code.hpp"

namespace example_cases {

int run() {
    // Demo 1：外部数组保存次数、代表位置、回文后缀数和每个位置的状态。
    std::string_view s = "ababa";
    Pam<> pam(int(s.size()));
    std::vector<long long> cnt(2);
    std::vector<int> pos(2, -1), dep(2), tail(s.size());
    std::array<int, 5> want{1, 1, 2, 2, 3};
    long long sum = 0;
    pam.add(s, [&](int p, int i) {
        cnt.resize(pam.len.size());
        pos.resize(pam.len.size(), -1);
        ++cnt[p];
        if (pos[p] < 0)
            pos[p] = i;
        if (p == int(dep.size()))
            dep.push_back(dep[pam.link[p]] + 1);
        tail[i] = p;
        assert(dep[p] == want[i]);
        sum += dep[p];
        std::cout << dep[p] << ' ';
    });
    std::cout << '\n';
    assert(sum == 9 and pam.len.size() - 2 == 5);

    // Demo 2：按节点编号逆序汇总，两个根不是实际回文答案。
    auto res = cnt;
    for (int p = int(res.size()) - 1; p >= 2; --p)
        res[pam.link[p]] += res[p];
    long long best = 0, num = 0;
    int a = pam.son[1][0];
    for (int p = 2; p < int(pam.len.size()); ++p) {
        int l = pos[p] - pam.len[p] + 1;
        auto text = std::string_view(pam.s).substr(l, pam.len[p]);
        std::cout << text << ": count=" << res[p] << '\n';
        best = std::max(best, res[p] * pam.len[p]);
        num += res[p];
    }
    assert(best == 6 and num == sum and res[a] == 3);
    std::cout << "total: " << sum << ", best: " << best << '\n';

    // Demo 3：单字符追加直接返回最长回文后缀，用反串求每个起点的长度。
    Pam<> rev;
    std::vector<int> head(s.size());
    for (int i = int(s.size()) - 1; i >= 0; --i)
        head[i] = rev.len[rev.add(s[i])];
    int pair = 0;
    for (int i = 1; i < int(s.size()); ++i)
        pair = std::max(pair, pam.len[tail[i - 1]] + head[i]);
    assert(pair == 4);
    std::cout << "adjacent palindromes: " << pair << '\n';

    // 整串下标从 0 开始，累计位置用本次追加前的长度加 i。
    int off = int(pam.s.size());
    pam.add("a", [&](int p, int i) {
        assert(i == 0);
        cnt.resize(pam.len.size());
        pos.resize(pam.len.size(), -1);
        ++cnt[p];
        if (pos[p] < 0)
            pos[p] = off + i;
    });
    auto now = cnt;
    for (int p = int(now.size()) - 1; p >= 2; --p)
        now[pam.link[p]] += now[p];
    assert(res[a] == 3 and now[a] == 4 and pam.len[pam.cur] == 2);
    return 0;
}
}

int main() {
    runCase("PalindromeAutomaton/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    runCase("PalindromeAutomaton/callbacks-and-byte-rejection", [] { CHECK(callback_cases::run() == 0); });
    runCase("PalindromeAutomaton/usage-example", [] { CHECK(example_cases::run() == 0); });
    return 0;
}
