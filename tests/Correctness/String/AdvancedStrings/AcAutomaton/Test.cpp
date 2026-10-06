#include "../../../../Support/CaseSupport.hpp"
#include "../../../../Support/AutomataSupport.hpp"


int coreCases() {
    return automata_test::suite(0);
}

#include "../../../../Support/AutomataSupport.hpp"

namespace boundary_cases {

int run() {
    runCase("AcAutomaton/empty-pattern-set", [] {
        automata_test::acCase<26, 'a'>({}, "");
    });
    runCase("AcAutomaton/empty-pattern", [] {
        automata_test::acCase<26, 'a'>({""}, "abc");
    });
    runCase("AcAutomaton/duplicate-overlaps", [] {
        automata_test::acCase<26, 'a'>({"a", "a", "aa"}, "aaaaaa");
    });
    runCase("AcAutomaton/suffix-failure-links", [] {
        automata_test::acCase<26, 'a'>({"he", "she", "hers", "his"}, "ahishers");
    });
    runCase("AcAutomaton/overlapping-patterns", [] {
        automata_test::acCase<26, 'a'>({"ab", "bc", "abc"}, "abcabcab");
    });
    runCase("AcAutomaton/disjoint-patterns", [] {
        automata_test::acCase<26, 'a'>({"abc", "def"}, "abcdef");
    });
    runCase("AcAutomaton/prefix-chain", [] {
        automata_test::acCase<26, 'a'>({"a", "ab", "abc", "abcd"}, "abcdabcab");
    });
    runCase("AcAutomaton/duplicate-empty-patterns", [] {
        automata_test::acCase<26, 'a'>({"", "", "aba"}, "ababa");
    });
    runCase("AcAutomaton/absent-pattern", [] {
        automata_test::acCase<26, 'a'>({"zzzz", "x"}, "xxxx");
    });
    runCase("AcAutomaton/periodic-failure-chain", [] {
        automata_test::acCase<26, 'a'>({"abcabc", "bcabc", "cabc"}, "abcabcabc");
    });
    return 0;
}
}

#include "../../../../../src/String/AdvancedStrings/AcAutomaton/code.hpp"
#include "../../../../Support/TestSupport.hpp"
#include <memory>

namespace callback_cases {

int run() {
    AcAutomaton<128, char(128)> a(0);
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
    a.build();
    int p = 0;
    for (char c : s)
        p = a.step(p, c);
    CHECK(p == seen[3].first and p == seen[7].first);
    return 0;
}
}

#include "../../../../../src/String/AdvancedStrings/AcAutomaton/code.hpp"

namespace example_cases {

int run() {
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
}

int main() {
    runCase("AcAutomaton/oracle-and-contracts", [] { CHECK(coreCases() == 0); });
    CHECK(boundary_cases::run() == 0);
    runCase("AcAutomaton/callbacks-and-byte-rejection", [] { CHECK(callback_cases::run() == 0); });
    runCase("AcAutomaton/usage-example", [] { CHECK(example_cases::run() == 0); });
    return 0;
}
