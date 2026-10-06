#pragma once
#include "BenchmarkSupport.hpp"
#include "../../src/String/AdvancedStrings/AcAutomaton/code.hpp"
#include "../../src/String/AdvancedStrings/SuffixAutomaton/code.hpp"
#include "../../src/String/AdvancedStrings/ExSuffixAutomaton/code.hpp"
#include "../../src/String/AdvancedStrings/PalindromeAutomaton/code.hpp"
#include <map>
#include <set>

namespace automata_bench {
std::uint64_t ac(const std::vector<std::string> &patterns, const std::string &text) {
    AcAutomaton<> a;
    std::vector<int> endpos;
    for (const auto &s : patterns)
        endpos.push_back(a.add(s));
    a.build();
    std::vector<std::uint64_t> cnt(a.son.size());
    cnt[0] = 1;
    int p = 0;
    for (char c : text) {
        p = a.step(p, c);
        ++cnt[p];
    }
    for (int i = int(a.order.size()) - 1; i > 0; --i) {
        int q = a.order[i];
        cnt[a.link[q]] += cnt[q];
    }
    std::uint64_t res = 0;
    for (int q : endpos)
        res += cnt[q];
    return res;
}

template <class A>
std::uint64_t query(const A &a, std::vector<std::uint64_t> cnt, const std::string &text) {
    auto ord = a.getOrder();
    std::uint64_t res = 0;
    for (int i = int(ord.size()) - 1; i > 0; --i) {
        int p = ord[i];
        cnt[a.link[p]] += cnt[p];
        res += a.len[p] - a.len[a.link[p]];
    }
    for (int i = 0; i < int(text.size()); i += 32)
        if (auto p = a.find(std::string_view(text).substr(i, 16)))
            res += cnt[*p];
    int p = 0, l = 0;
    for (auto it = text.rbegin(); it != text.rend(); ++it) {
        a.match(p, l, *it);
        res += l;
    }
    return res;
}

std::uint64_t sam(const std::string &text) {
    Sam<> a(int(text.size()));
    std::vector<std::uint64_t> cnt(1);
    a.add(text, [&](int p, int) {
        cnt.resize(a.len.size());
        ++cnt[p];
    });
    return query(a, std::move(cnt), text);
}

std::uint64_t ex(const std::vector<std::string> &patterns, const std::string &text) {
    ExSam<> a;
    std::vector<std::uint64_t> cnt(1);
    for (const auto &s : patterns)
        a.add(s, [&](int p, int) {
            cnt.resize(a.len.size());
            ++cnt[p];
        });
    return query(a, std::move(cnt), text);
}

std::uint64_t pam(const std::string &text) {
    Pam<> a(int(text.size()));
    std::vector<std::uint64_t> cnt(2);
    a.add(text, [&](int p, int) {
        cnt.resize(a.len.size());
        ++cnt[p];
    });
    std::uint64_t res = 0, best = 0;
    for (int p = int(cnt.size()) - 1; p >= 2; --p) {
        cnt[a.link[p]] += cnt[p];
        res += cnt[p];
        best = std::max(best, cnt[p] * std::uint64_t(a.len[p]));
    }
    return res + best;
}

std::uint64_t occurrences(const std::string &s, const std::string &t) {
    std::uint64_t n = 0;
    for (std::size_t i = 0; i + t.size() <= s.size(); ++i)
        n += s.compare(i, t.size(), t) == 0;
    return n;
}

std::uint64_t expected(int which, const std::vector<std::string> &patterns, const std::string &text) {
    if (which == 0) {
        std::uint64_t res = 0;
        for (const auto &s : patterns)
            res += occurrences(text, s);
        return res;
    }
    std::map<std::string, std::uint64_t> cnt;
    for (const auto &s : which == 2 ? patterns : std::vector<std::string>{text})
        for (std::size_t l = 0; l < s.size(); ++l)
            for (std::size_t r = l + 1; r <= s.size(); ++r) {
                auto t = s.substr(l, r - l);
                bool pal = true;
                for (std::size_t i = 0; i < t.size() / 2; ++i)
                    pal = pal and t[i] == t[t.size() - 1 - i];
                if (which != 3 or pal)
                    ++cnt[t];
            }
    if (which == 3) {
        std::uint64_t res = 0, best = 0;
        for (const auto &[s, n] : cnt) {
            res += n;
            best = std::max(best, n * s.size());
        }
        return res + best;
    }
    std::uint64_t res = cnt.size();
    for (std::size_t i = 0; i < text.size(); i += 32)
        res += cnt[text.substr(i, 16)];
    std::string rev(text.rbegin(), text.rend());
    for (std::size_t i = 0; i < rev.size(); ++i) {
        int best = 0;
        for (std::size_t l = 0; l <= i; ++l) {
            auto it = cnt.find(rev.substr(l, i + 1 - l));
            if (it != cnt.end() and it->second)
                best = std::max(best, int(i + 1 - l));
        }
        res += best;
    }
    return res;
}

int run(int which, int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    auto work = [which](const std::vector<std::string> &ps, const std::string &s) {
        if (which == 0) return ac(ps, s);
        if (which == 1) return sam(s);
        if (which == 2) return ex(ps, s);
        return pam(s);
    };
    // Independent enumeration is outside timing and uses the same measured functions.
    for (const auto &s : {std::string{}, std::string("abbababa"), std::string(48, 'a')}) {
        std::vector<std::string> ps{"", s, s, "ab", "bb"};
        if (work(ps, s) != expected(which, ps, s)) {
            std::cerr << "Benchmark oracle failed\n";
            return 1;
        }
    }
    std::mt19937_64 rng(input.seed);
    std::string text(input.n, 'a');
    for (char &c : text)
        c = input.shape ? 'a' : char('a' + rng() % 26);
    std::vector<std::string> patterns;
    for (int i = 0; i < input.n; i += 32)
        patterns.push_back(text.substr(i, 64));
    const char *label[] = {
        "random / repeated; AC build + scan + failure aggregation",
        "random / repeated; SAM build + occurrence aggregation + find + streaming match",
        "random / repeated sources; ExSAM build + aggregation + find + streaming match",
        "random / repeated; PAM build + occurrence aggregation + length-count maximum"
    };
    return measure(input, label[which], [&] { return work(patterns, text); });
}
} // namespace automata_bench
