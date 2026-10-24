#pragma once
#include "../../../../Headers/Headers.hpp"

template <class Info>
class ImplicitTreap {
    using u64 = unsigned long long;
    struct Node {
        int l = -1, r = -1, siz = 1;
        bool rev = false;
        u64 pri;
        Info val, pre, suf;

        Node(Info v, u64 pri) : pri(pri), val(std::move(v)), pre(val), suf(val) {}
    };
    std::vector<Node> t{};
    std::vector<int> free{};
    std::vector<std::pair<int, bool>> stk{};
    int rt = -1;
    u64 rng;

    int size(int p) const {
        return p == -1 ? 0 : t[p].siz;
    }

    void up(int p) {
        auto& v = t[p];
        v.siz = size(v.l) + 1 + size(v.r);
        v.pre = v.val;
        v.suf = v.val;
        if (v.l != -1) {
            v.pre = t[v.l].pre + v.pre;
            v.suf = v.suf + t[v.l].suf;
        }
        if (v.r != -1) {
            v.pre = v.pre + t[v.r].pre;
            v.suf = t[v.r].suf + v.suf;
        }
    }

    void flip(int p) {
        if (p != -1) {
            std::swap(t[p].l, t[p].r);
            std::swap(t[p].pre, t[p].suf);
            t[p].rev = not t[p].rev;
        }
    }

    void down(int p) {
        if (t[p].rev) {
            flip(t[p].l);
            flip(t[p].r);
            t[p].rev = false;
        }
    }

    std::pair<int, int> part(int p, int k) {
        stk.clear();
        while (p != -1) {
            down(p);
            int n = size(t[p].l);
            bool d = k > n;
            stk.push_back({p, d});
            if (d) {
                k -= n + 1;
                p = t[p].r;
            } else {
                p = t[p].l;
            }
        }
        int a = -1, b = -1;
        while (not stk.empty()) {
            auto [x, d] = stk.back();
            stk.pop_back();
            if (d) {
                t[x].r = a;
                up(x);
                a = x;
            } else {
                t[x].l = b;
                up(x);
                b = x;
            }
        }
        return {a, b};
    }

    int join(int a, int b) {
        stk.clear();
        while (a != -1 and b != -1) {
            if (t[a].pri >= t[b].pri) {
                down(a);
                stk.push_back({a, true});
                a = t[a].r;
            } else {
                down(b);
                stk.push_back({b, false});
                b = t[b].l;
            }
        }
        int p = a == -1 ? b : a;
        while (not stk.empty()) {
            auto [x, d] = stk.back();
            stk.pop_back();
            if (d) {
                t[x].r = p;
            } else {
                t[x].l = p;
            }
            up(x);
            p = x;
        }
        return p;
    }

    u64 next() {
        u64 x = (rng += 0x9e3779b97f4a7c15ULL);
        x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
        x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
        return x ^ (x >> 31);
    }

    void test(int l, int r) const {
        assert(0 <= l and l <= r and r <= size());
    }

public:
    explicit ImplicitTreap(u64 seed = std::chrono::steady_clock::now().time_since_epoch().count()) : rng(seed) {}

    explicit ImplicitTreap(const std::vector<Info>& a, u64 seed = std::chrono::steady_clock::now().time_since_epoch().count())
        : rng(seed) {
        assert(a.size() < std::size_t(std::numeric_limits<int>::max()));
        t.reserve(a.size());
        for (const auto& v : a) {
            int x = int(t.size()), p = -1;
            t.emplace_back(v, next());
            while (not stk.empty() and t[stk.back().first].pri < t[x].pri) {
                p = stk.back().first;
                stk.pop_back();
                up(p);
            }
            t[x].l = p;
            if (stk.empty()) {
                rt = x;
            } else {
                t[stk.back().first].r = x;
            }
            stk.push_back({x, false});
        }
        while (not stk.empty()) {
            up(stk.back().first);
            stk.pop_back();
        }
    }

    int size() const {
        return size(rt);
    }

    bool empty() const {
        return rt == -1;
    }

    void insert(int p, Info v) {
        test(p, p);
        int x;
        if (free.empty()) {
            assert(t.size() < std::size_t(std::numeric_limits<int>::max()));
            x = int(t.size());
            t.emplace_back(std::move(v), next());
        } else {
            x = free.back();
            free.pop_back();
            t[x] = Node(std::move(v), next());
        }
        auto [a, b] = part(rt, p);
        rt = join(join(a, x), b);
    }

    void erase(int l, int r) {
        test(l, r);
        auto [a, c] = part(rt, r);
        auto [b, m] = part(a, l);
        rt = join(b, c);
        std::size_t at = free.size();
        if (m != -1) {
            free.push_back(m);
        }
        for (; at < free.size(); ++at) {
            const auto& v = t[free[at]];
            if (v.l != -1) {
                free.push_back(v.l);
            }
            if (v.r != -1) {
                free.push_back(v.r);
            }
        }
    }

    void reverse(int l, int r) {
        test(l, r);
        auto [a, c] = part(rt, r);
        auto [b, m] = part(a, l);
        flip(m);
        rt = join(join(b, m), c);
    }

    Info query(int l, int r) {
        test(l, r);
        assert(l < r);
        auto [a, c] = part(rt, r);
        auto [b, m] = part(a, l);
        Info res = t[m].pre;
        rt = join(join(b, m), c);
        return res;
    }
};

// replace me: sequence sum.
struct Info {
    long long val;

    explicit Info(long long val = 0) : val(val) {}

    Info operator+(const Info& b) const {
        return Info(val + b.val);
    }
};
