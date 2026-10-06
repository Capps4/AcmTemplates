#pragma once
#include "CaseSupport.hpp"
#include "../../src/String/AdvancedStrings/AcAutomaton/code.hpp"
#include "../../src/String/AdvancedStrings/SuffixAutomaton/code.hpp"
#include "../../src/String/AdvancedStrings/ExSuffixAutomaton/code.hpp"
#include "../../src/String/AdvancedStrings/PalindromeAutomaton/code.hpp"

namespace automata_test {
std::array<int, 4> cases{};
std::string ctx{};
std::string hex(const std::string &s) {
    std::ostringstream out;
    for (unsigned char c : s)
        out << std::hex << std::setw(2) << std::setfill('0') << int(c);
    return out.str();
}
std::vector<std::string> words(int n) {
    std::vector<std::string> a{""};
    for (int len = 1; len <= n; ++len)
        for (int m = 0; m < (1 << len); ++m) {
            std::string s;
            for (int i = 0; i < len; ++i)
                s += char('a' + ((m >> i) & 1));
            a.push_back(s);
        }
    return a;
}
bool pal(std::string_view s) {
    for (int l = 0, r = int(s.size()) - 1; l < r; ++l, --r)
        if (s[l] != s[r])
            return false;
    return true;
}
std::map<std::string, long long> oracle(const std::string &s, bool onlyPal = false) {
    std::map<std::string, long long> res;
    for (int i = 0; i < int(s.size()); ++i)
        for (int j = i + 1; j <= int(s.size()); ++j) {
            auto t = s.substr(i, j - i);
            if (not onlyPal or pal(t))
                ++res[t];
        }
    return res;
}
long long occur(const std::string &s, const std::string &t) {
    long long n = 0;
    for (int i = 0; i + int(t.size()) <= int(s.size()); ++i)
        n += s.compare(i, t.size(), t) == 0;
    return n;
}
template <class A>
std::vector<int> order(const A &a) {
    auto ord = a.getOrder();
    CHECK(ord.size() == a.len.size() and ord[0] == 0);
    std::vector<int> rank(ord.size(), -1);
    for (int i = 0; i < int(ord.size()); ++i) {
        int p = ord[i];
        CHECK(0 <= p and p < int(rank.size()) and rank[p] == -1);
        rank[p] = i;
        if (i)
            CHECK(a.len[ord[i - 1]] <= a.len[p]);
    }
    CHECK(a.link[0] == -1);
    for (int p = 0; p < int(a.len.size()); ++p) {
        if (p) {
            CHECK(a.len[a.link[p]] < a.len[p]);
            CHECK(rank[a.link[p]] < rank[p]);
        }
        for (int q : a.son[p])
            if (q)
                CHECK(a.len[p] < a.len[q] and rank[p] < rank[q]);
    }
    return ord;
}
template <class A>
void matching(const A &a, const std::vector<std::string> &ss, const std::string &query) {
    int p = 0, l = 0;
    for (int i = 0; i < int(query.size()); ++i) {
        a.match(p, l, query[i]);
        int want = 0;
        for (int k = 1; k <= i + 1; ++k) {
            auto tail = query.substr(i + 1 - k, k);
            for (const auto &s : ss)
                if (s.find(tail) != std::string::npos)
                    want = k;
        }
        CHECK(l == want);
        CHECK((p == 0 and l == 0) or (p > 0 and a.len[a.link[p]] < l and l <= a.len[p]));
    }
}
template <int D = 2, char Base = 'a'>
void samCase(const std::string &s) {
    ++cases[1];
    ctx = "SAM " + hex(s);
    test_context::input = ctx;
    Sam<D, Base> a(0), single(0), plain(0);
    std::vector<long long> raw(1);
    std::vector<int> pos(1, -1), ends(s.size());
    int off = 0, total = 0;
    for (int n : {int(s.size()) / 2, int(s.size()) - int(s.size()) / 2}) {
        int seen = 0;
        a.add(std::string_view(s).substr(off, n), [&](int p, int i) {
            CHECK(i == seen++ and p == a.last and a.len[p] == off + i + 1);
            int old = int(raw.size());
            raw.resize(a.len.size());
            pos.resize(a.len.size(), -1);
            for (int q = old; q < int(raw.size()); ++q)
                CHECK(raw[q] == 0 and pos[q] == -1);
            ++raw[p];
            pos[p] = off + i;
            ends[off + i] = p;
            ++total;
        });
        CHECK(seen == n);
        off += n;
    }
    CHECK(total == int(s.size()));
    for (char c : s)
        single.add(c);
    plain.add(s);
    CHECK(a.son == single.son and a.link == single.link and a.len == single.len);
    CHECK(a.son == plain.son and a.link == plain.link and a.len == plain.len);
    auto ord = order(a);
    auto res = raw;
    auto loc = pos;
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int p = ord[i], f = a.link[p];
        res[f] += res[p];
        if (loc[f] < 0)
            loc[f] = loc[p];
    }
    auto root = a.find("");
    CHECK(res[0] == int(s.size()) and root and *root == 0);
    std::set<int> actual(ends.begin(), ends.end());
    for (int p = 1; p < int(raw.size()); ++p)
        CHECK(raw[p] == (actual.count(p) ? 1 : 0));
    auto want = oracle(s);
    long long distinct = 0;
    for (int p = 1; p < int(a.len.size()); ++p)
        distinct += a.len[p] - a.len[a.link[p]];
    CHECK(distinct == int(want.size()));
    for (const auto &[t, cnt] : want) {
        auto p = a.find(t);
        CHECK(p and *p > 0 and res[*p] == cnt);
        CHECK(loc[*p] + 1 >= int(t.size()) and loc[*p] < int(s.size()));
        CHECK(s.substr(loc[*p] + 1 - t.size(), t.size()) == t);
    }
    if constexpr (D == 2) {
        for (const auto &t : words(4))
            CHECK(a.find(t).has_value() == (s.find(t) != std::string::npos));
        matching(a, {s}, "abbabbaab");
    } else if constexpr (D == 256)
        matching(a, {s}, s + std::string("\0\xff\x80", 3));
}
template <int D = 2, char Base = 'a'>
void exCase(const std::vector<std::string> &ss) {
    ++cases[2];
    ctx = "ExSAM";
    for (const auto &s : ss)
        ctx += " " + hex(s);
    ExSam<D, Base> a(0), plain(0);
    std::vector<long long> raw(1);
    std::vector<unsigned> src(1);
    std::map<std::string, long long> want;
    std::map<std::string, unsigned> masks;
    int total = 0;
    for (int id = 0; id < int(ss.size()); ++id) {
        int seen = 0;
        int end = a.add(ss[id], [&](int p, int i) {
            CHECK(i == seen++ and a.len[p] == i + 1);
            raw.resize(a.len.size());
            src.resize(a.len.size());
            ++raw[p];
            src[p] |= 1U << id;
        });
        CHECK(seen == int(ss[id].size()));
        CHECK(a.len[end] == int(ss[id].size()));
        plain.add(ss[id]);
        total += seen;
        for (const auto &[t, cnt] : oracle(ss[id])) {
            want[t] += cnt;
            masks[t] |= 1U << id;
        }
    }
    CHECK(a.son == plain.son and a.link == plain.link and a.len == plain.len);
    auto ord = order(a);
    auto res = raw;
    auto mask = src;
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int p = ord[i], f = a.link[p];
        res[f] += res[p];
        mask[f] |= mask[p];
    }
    auto root = a.find("");
    CHECK(res[0] == total and root and *root == 0);
    long long distinct = 0;
    for (int p = 1; p < int(a.len.size()); ++p)
        distinct += a.len[p] - a.len[a.link[p]];
    CHECK(distinct == int(want.size()));
    for (const auto &[t, cnt] : want) {
        auto p = a.find(t);
        CHECK(p and *p > 0 and res[*p] == cnt and mask[*p] == masks[t]);
    }
    if constexpr (D == 2) {
        for (const auto &t : words(4))
            CHECK(a.find(t).has_value() == (t.empty() or want.count(t)));
        matching(a, ss, "abbabbaab");
    } else if constexpr (D == 256)
        matching(a, ss, std::string("\0\xff\x80\0\x01", 5));
}
template <int D = 2, char Base = 'a'>
void acCase(const std::vector<std::string> &ps, const std::string &text) {
    ++cases[0];
    ctx = "AC text=" + hex(text);
    AcAutomaton<D, Base> a(0), plain(0);
    std::vector<int> ends, depth(1);
    std::vector<long long> raw(1);
    std::vector<std::string> rep(1);
    std::map<std::string, int> prefixes{{"", 0}};
    for (int id = 0; id < int(ps.size()); ++id) {
        const auto &s = ps[id];
        int seen = 0;
        int p = a.add(s, [&](int p, int i) {
            CHECK(i == seen++);
            raw.resize(a.son.size());
            depth.resize(a.son.size());
            rep.resize(a.son.size());
            depth[p] = i + 1;
            rep[p] = s.substr(0, i + 1);
            prefixes[rep[p]] = p;
            if (i + 1 == int(s.size()))
                raw[p] += id + 1;
        });
        CHECK(seen == int(s.size()));
        if (s.empty())
            raw[p] += id + 1;
        ends.push_back(p);
        CHECK(plain.add(s) == p);
    }
    CHECK(a.son == plain.son);
    a.build();
    plain.build();
    CHECK(a.son == plain.son and a.link == plain.link and a.order == plain.order);
    auto son = a.son;
    auto link = a.link;
    auto ord = a.order;
    a.build();
    CHECK(a.son == son and a.link == link and a.order == ord);
    CHECK(ord.size() == a.son.size() and ord[0] == 0 and a.link[0] == 0);
    std::vector<int> rank(ord.size(), -1);
    for (int i = 0; i < int(ord.size()); ++i) {
        int p = ord[i];
        CHECK(rank[p] == -1);
        rank[p] = i;
        if (i)
            CHECK(depth[ord[i - 1]] <= depth[p]);
    }
    for (int p = 1; p < int(a.son.size()); ++p) {
        int want = 0;
        for (int k = 1; k < int(rep[p].size()); ++k) {
            auto it = prefixes.find(rep[p].substr(k));
            if (it != prefixes.end()) {
                want = it->second;
                break;
            }
        }
        CHECK(a.link[p] == want and rank[want] < rank[p]);
    }
    auto hit = raw;
    for (int i = 1; i < int(ord.size()); ++i) {
        int p = ord[i];
        hit[p] += hit[a.link[p]];
    }
    std::vector<long long> cnt(a.son.size());
    cnt[0] = 1;
    long long weighted = hit[0];
    int p = 0;
    for (char c : text) {
        p = a.step(p, c);
        ++cnt[p];
        weighted += hit[p];
    }
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int q = ord[i];
        cnt[a.link[q]] += cnt[q];
    }
    long long expected = 0;
    for (int id = 0; id < int(ps.size()); ++id) {
        long long n = occur(text, ps[id]);
        CHECK(cnt[ends[id]] == n);
        expected += (id + 1) * n;
    }
    CHECK(weighted == expected and cnt[0] == int(text.size()) + 1);
}
template <int D = 2, char Base = 'a'>
void pamCase(const std::string &s) {
    ++cases[3];
    ctx = "PAM " + hex(s);
    test_context::input = ctx;
    Pam<D, Base> a(0), single(0), plain(0);
    std::vector<long long> raw(2);
    std::vector<int> pos(2, -1), dep(2);
    int off = 0, total = 0;
    for (int n : {int(s.size()) / 2, int(s.size()) - int(s.size()) / 2}) {
        int seen = 0;
        a.add(std::string_view(s).substr(off, n), [&](int p, int i) {
            CHECK(i == seen++ and p == a.cur and int(a.s.size()) == off + i + 1);
            raw.resize(a.len.size());
            pos.resize(a.len.size(), -1);
            ++raw[p];
            if (pos[p] < 0)
                pos[p] = off + i;
            if (p == int(dep.size()))
                dep.push_back(dep[a.link[p]] + 1);
            int want = 0, count = 0;
            auto prefix = std::string_view(s).substr(0, off + i + 1);
            for (int k = 1; k <= int(prefix.size()); ++k)
                if (pal(prefix.substr(prefix.size() - k))) {
                    want = k;
                    ++count;
                }
            CHECK(a.len[p] == want and dep[p] == count);
            ++total;
        });
        CHECK(seen == n);
        off += n;
    }
    CHECK(total == int(s.size()) and a.link[0] == 1 and a.link[1] == 1);
    for (char c : s)
        single.add(c);
    plain.add(s);
    CHECK(a.son == single.son and a.link == single.link and a.len == single.len);
    CHECK(a.son == plain.son and a.link == plain.link and a.len == plain.len);
    auto res = raw;
    for (int p = int(res.size()) - 1; p >= 2; --p) {
        CHECK(a.link[p] < p and a.len[a.link[p]] < a.len[p]);
        res[a.link[p]] += res[p];
    }
    auto want = oracle(s, true);
    CHECK(a.len.size() == want.size() + 2);
    std::set<std::string> got;
    for (int p = 2; p < int(a.len.size()); ++p) {
        CHECK(pos[p] >= a.len[p] - 1);
        auto t = s.substr(pos[p] + 1 - a.len[p], a.len[p]);
        CHECK(pal(t) and want.count(t) and res[p] == want.at(t));
        got.insert(t);
    }
    CHECK(got.size() == want.size());
    for (int p = 0; p < int(a.len.size()); ++p)
        for (int q : a.son[p])
            if (q)
                CHECK(a.len[q] == a.len[p] + 2);
}
void branches() {
    ++cases[2];
    ctx = "ExSAM branching prefixes";
    ExSam<2> a;
    std::map<std::string, int> node{{"", 0}};
    std::map<std::string, long long> want;
    std::vector<long long> raw(1);
    for (const auto &s : words(6)) {
        if (s.empty())
            continue;
        int p = a.add(node.at(s.substr(0, s.size() - 1)), s.back());
        node[s] = p;
        raw.resize(a.len.size());
        ++raw[p];
        for (int k = 1; k <= int(s.size()); ++k)
            ++want[s.substr(s.size() - k)];
    }
    auto ord = order(a);
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int p = ord[i];
        raw[a.link[p]] += raw[p];
    }
    for (const auto &[s, n] : want) {
        auto p = a.find(s);
        CHECK(p and raw[*p] == n);
    }
}
void large(int which) {
    int n = 128;
    std::string s(n, 'a');
    if (which == 1) {
        ++cases[1];
        ctx = "SAM bounded repeated chars";
        Sam<2> a(1);
        std::vector<long long> raw(1);
        a.add(s, [&](int p, int i) {
            CHECK(a.len[p] == i + 1);
            raw.resize(a.len.size());
            ++raw[p];
        });
        auto ord = order(a);
        for (int i = int(ord.size()) - 1; i > 0; --i) {
            int p = ord[i];
            raw[a.link[p]] += raw[p];
        }
        CHECK(a.len.size() == std::size_t(n + 1));
        for (int p = 1; p <= n; ++p)
            CHECK(raw[p] == n - a.len[p] + 1);
    }
    if (which == 1) {
        ++cases[1];
        ctx = "SAM bounded clone-heavy chars";
        Sam<2> a(1);
        std::vector<long long> raw(1);
        std::string text = "a" + std::string(n, 'b');
        int calls = 0;
        a.add(text, [&](int p, int i) {
            CHECK(i == calls++);
            raw.resize(a.len.size());
            ++raw[p];
        });
        auto b = a.find("b");
        CHECK(calls == n + 1 and b and raw[*b] == 0);
        auto ord = order(a);
        long long distinct = 0;
        for (int p = 1; p < int(a.len.size()); ++p)
            distinct += a.len[p] - a.len[a.link[p]];
        CHECK(distinct == 2LL * n + 1);
        for (int i = int(ord.size()) - 1; i > 0; --i) {
            int p = ord[i];
            raw[a.link[p]] += raw[p];
        }
        for (int k : {1, 2, 17, 64, n}) {
            auto tail = a.find(std::string(k, 'b'));
            auto full = a.find("a" + std::string(k, 'b'));
            CHECK(tail and raw[*tail] == n - k + 1);
            CHECK(full and raw[*full] == 1);
        }
    }
    if (which == 0) {
        ++cases[0];
        ctx = "AC bounded deep failure chain";
        AcAutomaton<2> a(1);
        int end = a.add(s);
        a.add("");
        a.build();
        std::vector<long long> cnt(a.son.size());
        cnt[0] = 1;
        int p = 0;
        for (char c : s) {
            p = a.step(p, c);
            ++cnt[p];
        }
        for (int i = int(a.order.size()) - 1; i > 0; --i) {
            int q = a.order[i];
            cnt[a.link[q]] += cnt[q];
        }
        CHECK(cnt[end] == 1 and cnt[0] == n + 1);
        for (int p = 1; p <= n; ++p)
            CHECK(cnt[p] == n - p + 1);
    }
    if (which == 2) {
        ++cases[2];
        ctx = "ExSAM bounded repeated duplicate source";
        ExSam<2> a(1);
        std::vector<long long> raw(1);
        for (int k = 0; k < 2; ++k)
            a.add(s, [&](int p, int i) {
                CHECK(a.len[p] == i + 1);
                raw.resize(a.len.size());
                ++raw[p];
            });
        auto ord = order(a);
        for (int i = int(ord.size()) - 1; i > 0; --i) {
            int p = ord[i];
            raw[a.link[p]] += raw[p];
        }
        CHECK(a.len.size() == std::size_t(n + 1));
        for (int p = 1; p <= n; ++p)
            CHECK(raw[p] == 2LL * (n - a.len[p] + 1));
    }
    if (which == 3) {
        ++cases[3];
        ctx = "PAM bounded deep suffix chain";
        Pam<2> a(1);
        std::vector<long long> raw(2);
        a.add(s, [&](int p, int i) {
            CHECK(a.len[p] == i + 1);
            raw.resize(a.len.size());
            ++raw[p];
        });
        for (int p = int(raw.size()) - 1; p >= 2; --p)
            raw[a.link[p]] += raw[p];
        long long sum = 0;
        for (int p = 2; p < int(raw.size()); ++p) {
            CHECK(raw[p] == n - a.len[p] + 1);
            sum += raw[p];
        }
        CHECK(sum == 1LL * n * (n + 1) / 2);
    }
}

int suite(int which) {
    const std::vector<std::string> fixed{"", "a", "b", "aa", "ab", "ba", "bb", "abb", "abbb", "aba", "ababa", "aaaaaa", "ababab", "aabbab", "bbabba"};
    for (const auto &s : fixed) {
        if (which == 0) acCase({"", s, s, "a", "b"}, s + s);
        if (which == 1) samCase(s);
        if (which == 2) exCase({"", s, s, "ab", "ba"});
        if (which == 3) pamCase(s);
    }
    if (which == 1)
        for (const auto &s : words(4)) samCase(s);
    if (which == 3)
        for (const auto &s : words(5)) pamCase(s);
    if (which == 0) {
        auto pool = words(2);
        for (const auto &a : pool)
            for (const auto &b : pool)
                for (const auto &s : pool) acCase({a, b, a, ""}, s);
        acCase({}, "abba");
    }
    if (which == 2) {
        auto pool = words(2);
        for (const auto &a : pool)
            for (const auto &b : pool) exCase({a, b});
        exCase({});
        exCase({"", "", ""});
        branches();
    }
    auto randText = [](int n, int d) {
        std::string s;
        for (int i = 0; i < n; ++i) s += char('a' + randomInt(0, d - 1));
        return s;
    };
    for (int it = 0; it < 8; ++it) {
        auto s = randText(randomInt(0, 40), 2);
        if (which == 1) samCase(s);
        if (which == 3) pamCase(s);
        std::vector<std::string> group;
        for (int j = 0, n = randomInt(0, 6); j < n; ++j)
            group.push_back(randText(randomInt(0, 20), 2));
        if (which == 2) exCase(group);
        if (which == 0) acCase(group, randText(randomInt(0, 60), 2));
    }
    for (int it = 0; it < 8; ++it) {
        std::string a, b;
        for (int i = 0, n = randomInt(0, 24); i < n; ++i) a += char(randomInt(0, 255));
        for (int i = 0, n = randomInt(0, 24); i < n; ++i) b += char(randomInt(0, 255));
        a += std::string("\0\xff\x80\0", 4);
        if (which == 0) acCase<256, 0>({"", a, b, a}, b + a);
        if (which == 1) samCase<256, 0>(a);
        if (which == 2) exCase<256, 0>({"", a, b, a});
        if (which == 3) pamCase<256, 0>(a);
    }
    large(which);
    std::cout << "AUTOMATON_CASES " << cases[which] << " PASS\n";
    return 0;
}
} // namespace automata_test
