#pragma once
#include "../../../../Headers/Headers.hpp"

class SegmentTreeBeats {
    using i64 = long long;
    using i128 = __int128_t;
    struct Node {
        i64 sum = 0, mn = 0, mx = 0;
        i64 smn = 0, smx = 0;
        i128 add = 0;
        int len = 0, cmn = 0, cmx = 0;
    };
    int n;
    std::vector<Node> t;

    void up(int p) {
        auto& v = t[p];
        const auto& a = t[p * 2];
        const auto& b = t[p * 2 + 1];
        v.sum = i64(i128(a.sum) + b.sum);
        v.len = a.len + b.len;
        v.mn = std::min(a.mn, b.mn);
        v.mx = std::max(a.mx, b.mx);
        v.cmn = (a.mn == v.mn ? a.cmn : 0) + (b.mn == v.mn ? b.cmn : 0);
        v.cmx = (a.mx == v.mx ? a.cmx : 0) + (b.mx == v.mx ? b.cmx : 0);
        v.smn = std::min(a.mn == v.mn ? a.smn : a.mn, b.mn == v.mn ? b.smn : b.mn);
        v.smx = std::max(a.mx == v.mx ? a.smx : a.mx, b.mx == v.mx ? b.smx : b.mx);
    }

    void init(int p, int l, int r, const std::vector<i64>& a) {
        if (r - l == 1) {
            t[p] = {a[l], a[l], a[l], std::numeric_limits<i64>::max(),
                    std::numeric_limits<i64>::lowest(), 0, 1, 1, 1};
            return;
        }
        int m = l + (r - l) / 2;
        init(p * 2, l, m, a);
        init(p * 2 + 1, m, r, a);
        up(p);
    }

    void cap(int p, i64 x) {
        auto& v = t[p];
        v.sum = i64(i128(v.sum) + (i128(x) - v.mx) * v.cmx);
        if (v.mn == v.mx) {
            v.mn = x;
        } else if (v.smn == v.mx) {
            v.smn = x;
        }
        v.mx = x;
    }

    void low(int p, i64 x) {
        auto& v = t[p];
        v.sum = i64(i128(v.sum) + (i128(x) - v.mn) * v.cmn);
        if (v.mn == v.mx) {
            v.mx = x;
        } else if (v.smx == v.mn) {
            v.smx = x;
        }
        v.mn = x;
    }

    void plus(int p, i64 x) {
        auto& v = t[p];
        v.sum = i64(i128(v.sum) + i128(x) * v.len);
        v.mn += x;
        v.mx += x;
        if (v.cmn < v.len) {
            v.smn += x;
        }
        if (v.cmx < v.len) {
            v.smx += x;
        }
        v.add += x;
    }

    void down(int p) {
        auto& v = t[p];
        for (int c : {p * 2, p * 2 + 1}) {
            auto& u = t[c];
            i128 lo = i128(u.mn) + v.add, hi = i128(u.mx) + v.add;
            i128 sl = u.smn, sh = u.smx, sum = i128(u.sum) + v.add * u.len;
            if (u.cmn < u.len) {
                sl += v.add;
            }
            if (u.cmx < u.len) {
                sh += v.add;
            }
            // Stale child extrema can leave i64 before the parent's clamps apply.
            if (hi > v.mx) {
                sum += (i128(v.mx) - hi) * u.cmx;
                if (lo == hi) {
                    lo = v.mx;
                } else if (sl == hi) {
                    sl = v.mx;
                }
                hi = v.mx;
            }
            if (lo < v.mn) {
                sum += (i128(v.mn) - lo) * u.cmn;
                if (lo == hi) {
                    hi = v.mn;
                } else if (sh == lo) {
                    sh = v.mn;
                }
                lo = v.mn;
            }
            u.sum = i64(sum);
            u.mn = i64(lo);
            u.mx = i64(hi);
            u.smn = i64(sl);
            u.smx = i64(sh);
            u.add += v.add;
        }
        v.add = 0;
    }

    void edit(int p, int l, int r, int ql, int qr, i64 x, int op) {
        if (r <= ql or qr <= l or (op == 0 and t[p].mx <= x) or (op == 1 and t[p].mn >= x)) {
            return;
        }
        if (ql <= l and r <= qr) {
            if (op == 2) {
                plus(p, x);
                return;
            }
            if (op == 0 and (t[p].cmx == t[p].len or t[p].smx < x)) {
                cap(p, x);
                return;
            }
            if (op == 1 and (t[p].cmn == t[p].len or x < t[p].smn)) {
                low(p, x);
                return;
            }
        }
        down(p);
        int m = l + (r - l) / 2;
        edit(p * 2, l, m, ql, qr, x, op);
        edit(p * 2 + 1, m, r, ql, qr, x, op);
        up(p);
    }

    void test(int l, int r) const {
        assert(0 <= l and l <= r and r <= n);
    }

public:
    struct Result {
        i64 sum, mn, mx;
    };

    explicit SegmentTreeBeats(const std::vector<i64>& a)
        : n(int(a.size())), t(4 * std::max(std::size_t(1), a.size())) {
        assert(a.size() <= std::size_t(std::numeric_limits<int>::max() / 4));
        if (n != 0) {
            init(1, 0, n, a);
        }
    }

    void chmin(int l, int r, i64 x) {
        test(l, r);
        if (l < r) {
            edit(1, 0, n, l, r, x, 0);
        }
    }

    void chmax(int l, int r, i64 x) {
        test(l, r);
        if (l < r) {
            edit(1, 0, n, l, r, x, 1);
        }
    }

    void add(int l, int r, i64 x) {
        test(l, r);
        if (l < r) {
            edit(1, 0, n, l, r, x, 2);
        }
    }

    Result query(int l, int r) {
        test(l, r);
        assert(l < r);
        return qry(1, 0, n, l, r);
    }

private:
    Result qry(int p, int l, int r, int ql, int qr) {
        if (ql <= l and r <= qr) {
            return {t[p].sum, t[p].mn, t[p].mx};
        }
        down(p);
        int m = l + (r - l) / 2;
        if (qr <= m) {
            return qry(p * 2, l, m, ql, qr);
        }
        if (m <= ql) {
            return qry(p * 2 + 1, m, r, ql, qr);
        }
        auto a = qry(p * 2, l, m, ql, qr);
        auto b = qry(p * 2 + 1, m, r, ql, qr);
        return {i64(i128(a.sum) + b.sum), std::min(a.mn, b.mn), std::max(a.mx, b.mx)};
    }
};
