#pragma once
#include <bits/stdc++.h>
using i64 = long long;
template <class T, class Tag>
class LazySegT {
    int n;
    std::vector<T> info{};
    std::vector<Tag> tag{};

    void up(int i) {
        info[i] = info[i * 2] + info[i * 2 + 1];
    }
    void apply(int i, const Tag &v) {
        info[i].apply(v);
        tag[i].apply(v);
    }
    void down(int i) {
        apply(i * 2, tag[i]);
        apply(i * 2 + 1, tag[i]);
        tag[i] = Tag();
    }

    void modify(int i, int l, int r, int x, const T &v) {
        if (r - l == 1) {
            info[i] = v;
            return;
        }
        int mid = (l + r) / 2;
        down(i);
        if (x < mid) {
            modify(i * 2, l, mid, x, v);
        } else {
            modify(i * 2 + 1, mid, r, x, v);
        }
        up(i);
    }

    T rangeQuery(int i, int l, int r, int tl, int tr) {

        if (tl <= l and r <= tr)
            return info[i];

        down(i);
        int mid = (l + r) / 2;

        if (tl < mid and mid < tr) {
            return rangeQuery(i * 2, l, mid, tl, tr) + rangeQuery(i * 2 + 1, mid, r, tl, tr);
        } else {
            return (tl < mid ? rangeQuery(i * 2, l, mid, tl, tr) : rangeQuery(i * 2 + 1, mid, r, tl, tr));
        }
    }

    void rangeModify(int i, int l, int r, int tl, int tr, const Tag &v) {

        if (tl <= l and r <= tr) {
            apply(i, v);
            return;
        }
        down(i);
        int mid = (l + r) / 2;

        if (tl < mid)
            rangeModify(i * 2, l, mid, tl, tr, v);
        if (mid < tr)
            rangeModify(i * 2 + 1, mid, r, tl, tr, v);
        up(i);
    }

    template <class F>
    int findFirst(int i, int l, int r, int tl, int tr, F pred) {
        if (l >= tr || r <= tl || !pred(info[i])) {
            return -1;
        }
        if (r - l == 1) {
            return l;
        }
        int mid = (l + r) / 2;
        down(i);
        int res = findFirst(i * 2, l, mid, tl, tr, pred);
        if (res == -1) {
            res = findFirst(i * 2 + 1, mid, r, tl, tr, pred);
        }
        return res;
    }

    template <class F>
    int findLast(int i, int l, int r, int tl, int tr, F pred) {
        if (l >= tr || r <= tl || !pred(info[i])) {
            return -1;
        }
        if (r - l == 1) {
            return l;
        }
        int mid = (l + r) / 2;
        down(i);
        int res = findLast(i * 2 + 1, mid, r, tl, tr, pred);
        if (res == -1) {
            res = findLast(i * 2, l, mid, tl, tr, pred);
        }
        return res;
    }
public:
    LazySegT(int n, T v = T())
        : LazySegT(std::vector<T>(n, v)) {}

    template <class G>
    LazySegT(const std::vector<G> &a) : n(a.size()) {
        info.assign(4 << std::__lg(n), T());
        tag.assign(4 << std::__lg(n), Tag());
        std::function<void(int, int, int)> build = [&](int i, int l, int r) {
            if (r - l == 1) {
                info[i] = T(a[l]);
                return;
            }
            int mid = (l + r) / 2;
            build(i * 2, l, mid);
            build(i * 2 + 1, mid, r);
            up(i);
        };
        build(1, 0, n);
    }

    // 单点赋值
    void modify(int i, const T &v) {
        modify(1, 0, n, i, v);
    }

    // 区间查询 [l, r)
    T rangeQuery(int l, int r) {
        return rangeQuery(1, 0, n, l, r);
    }

    // 区间修改 [l, r)
    void rangeModify(int l, int r, const Tag &v) {
        return rangeModify(1, 0, n, l, r, v);
    }

    // 区间左边第一个满足条件的下标
    template <class F>
    int findFirst(int l, int r, F pred) {
        return findFirst(1, 0, n, l, r, pred);
    }

    // 区间右边第一个满足条件的下标
    template <class F>
    int findLast(int l, int r, F pred) {
        return findLast(1, 0, n, l, r, pred);
    }
};

struct Tag {
    int add;

    explicit Tag(int add = 0)
        : add(add){}

    void apply(const Tag &tag) {
        if (tag.add) {
            add += tag.add;
        }
    }
};

struct Node {
    i64 val;
    int len;

    explicit Node(i64 val = 0, int len = 1)
        : val(val), len(len) {}

    void apply(const Tag &tag) {
        if (tag.add) {
            val += 1LL * tag.add * len;
        }
    }

    Node operator+(const Node &a) {
        return Node(val + a.val, len + a.len);
    }
};

