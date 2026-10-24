#pragma once
#include "../../../../Headers/Headers.hpp"

template <class Info>
class LinkCutTree {
    struct Node {
        int fa = -1;
        int ch[2]{-1, -1};
        bool rev = false;
        Info val, pre, suf;

        explicit Node(Info v) : val(std::move(v)), pre(val), suf(val) {}
    };
    std::vector<Node> t{};
    std::vector<int> stk{};

    bool top(int x) const {
        int p = t[x].fa;
        return p == -1 or (t[p].ch[0] != x and t[p].ch[1] != x);
    }

    void up(int x) {
        auto& v = t[x];
        v.pre = v.val;
        v.suf = v.val;
        int l = v.ch[0], r = v.ch[1];
        if (l != -1) {
            v.pre = t[l].pre + v.pre;
            v.suf = v.suf + t[l].suf;
        }
        if (r != -1) {
            v.pre = v.pre + t[r].pre;
            v.suf = t[r].suf + v.suf;
        }
    }

    void flip(int x) {
        if (x != -1) {
            std::swap(t[x].ch[0], t[x].ch[1]);
            std::swap(t[x].pre, t[x].suf);
            t[x].rev = not t[x].rev;
        }
    }

    void down(int x) {
        if (t[x].rev) {
            flip(t[x].ch[0]);
            flip(t[x].ch[1]);
            t[x].rev = false;
        }
    }

    void rot(int x) {
        int y = t[x].fa, z = t[y].fa;
        int d = t[y].ch[1] == x, b = t[x].ch[!d];
        if (not top(y)) {
            t[z].ch[t[z].ch[1] == y] = x;
        }
        t[x].fa = z;
        t[x].ch[!d] = y;
        t[y].fa = x;
        t[y].ch[d] = b;
        if (b != -1) {
            t[b].fa = y;
        }
        up(y);
        up(x);
    }

    void sply(int x) {
        stk.clear();
        int y = x;
        stk.push_back(y);
        while (not top(y)) {
            y = t[y].fa;
            stk.push_back(y);
        }
        for (auto it = stk.rbegin(); it != stk.rend(); ++it) {
            down(*it);
        }
        while (not top(x)) {
            y = t[x].fa;
            int z = t[y].fa;
            if (not top(y)) {
                bool same = (t[y].ch[1] == x) == (t[z].ch[1] == y);
                rot(same ? y : x);
            }
            rot(x);
        }
    }

    void acc(int x) {
        int last = -1;
        for (int y = x; y != -1; y = t[y].fa) {
            sply(y);
            t[y].ch[1] = last;
            if (last != -1) {
                t[last].fa = y;
            }
            up(y);
            last = y;
        }
        sply(x);
    }

    void root(int x) {
        acc(x);
        flip(x);
    }

    int find(int x) {
        acc(x);
        down(x);
        while (t[x].ch[0] != -1) {
            x = t[x].ch[0];
            down(x);
        }
        sply(x);
        return x;
    }

    void test(int x) const {
        assert(0 <= x and x < int(t.size()));
    }

public:
    explicit LinkCutTree(const std::vector<Info>& a) {
        assert(a.size() <= std::size_t(std::numeric_limits<int>::max()));
        t.reserve(a.size());
        for (const auto& v : a) {
            t.emplace_back(v);
        }
    }

    bool connected(int x, int y) {
        test(x);
        test(y);
        return x == y or find(x) == find(y);
    }

    bool link(int x, int y) {
        test(x);
        test(y);
        root(x);
        if (find(y) == x) {
            return false;
        }
        t[x].fa = y;
        return true;
    }

    bool cut(int x, int y) {
        test(x);
        test(y);
        root(x);
        acc(y);
        if (t[y].ch[0] != x or t[x].ch[1] != -1) {
            return false;
        }
        t[y].ch[0] = -1;
        t[x].fa = -1;
        up(y);
        return true;
    }

    void modify(int x, Info v) {
        test(x);
        acc(x);
        t[x].val = std::move(v);
        up(x);
    }

    std::optional<Info> query(int x, int y) {
        test(x);
        test(y);
        root(x);
        if (find(y) != x) {
            return std::nullopt;
        }
        acc(y);
        return t[y].pre;
    }
};

// replace me: vertex sum on a directed path.
struct Info {
    long long val;

    explicit Info(long long val = 0) : val(val) {}

    Info operator+(const Info& b) const {
        return Info(val + b.val);
    }
};
