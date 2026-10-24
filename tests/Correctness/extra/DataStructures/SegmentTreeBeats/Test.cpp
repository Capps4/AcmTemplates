#include "../../../../../src/extra/DataStructures/SegmentTreeBeats/code.hpp"
#include "../../../../Support/CaseSupport.hpp"

void checkRange(SegmentTreeBeats& t, const std::vector<long long>& a, int l, int r) {
    auto value = t.query(l, r);
    __int128 sum = 0;
    for (int i = l; i < r; ++i) {
        sum += a[i];
    }
    CHECK(value.sum == sum);
    CHECK(value.mn == *std::min_element(a.begin() + l, a.begin() + r));
    CHECK(value.mx == *std::max_element(a.begin() + l, a.begin() + r));
}

void apply(SegmentTreeBeats& t, std::vector<long long>& a, int l, int r, long long v, int op) {
    if (op == 0) {
        t.chmin(l, r, v);
    } else if (op == 1) {
        t.chmax(l, r, v);
    } else {
        t.add(l, r, v);
    }
    for (int i = l; i < r; ++i) {
        a[i] = op == 0 ? std::min(a[i], v) : op == 1 ? std::max(a[i], v) : a[i] + v;
    }
}

int main() {
    runCase("SegmentTreeBeats/exhaustive-small", [] {
        SegmentTreeBeats empty({});
        empty.chmin(0, 0, 1);
        empty.chmax(0, 0, -1);
        empty.add(0, 0, 3);
        for (int n = 1, count = 3; n <= 4; ++n, count *= 3) {
            for (int code = 0; code < count; ++code) {
                int value = code;
                std::vector<long long> a(n);
                for (auto& x : a) {
                    x = value % 3 - 1;
                    value /= 3;
                }
                for (int l = 0; l <= n; ++l) {
                    for (int r = l; r <= n; ++r) {
                        for (int op = 0; op < 3; ++op) {
                            for (long long v : {-2, -1, 0, 1, 2}) {
                                auto b = a;
                                SegmentTreeBeats t(b);
                                apply(t, b, l, r, v, op);
                                for (int i = 0; i < n; ++i) {
                                    for (int j = i + 1; j <= n; ++j) {
                                        checkRange(t, b, i, j);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    });
    runCase("SegmentTreeBeats/random-oracle", [] {
        for (int trial = 0; trial < 20; ++trial) {
            int n = randomInt(1, 60);
            std::vector<long long> a(n);
            for (auto& x : a) {
                x = randomInt(-30, 30);
            }
            SegmentTreeBeats t(a);
            for (int step = 0; step < 400; ++step) {
                test_context::step = step;
                int l = randomInt(0, n), r = randomInt(l, n);
                apply(t, a, l, r, randomInt(-40, 40), randomInt(0, 2));
                checkRange(t, a, 0, n);
                l = randomInt(0, n - 1);
                r = randomInt(l + 1, n);
                checkRange(t, a, l, r);
            }
        }
    });
    runCase("SegmentTreeBeats/extreme-values", [] {
        const auto lo = std::numeric_limits<long long>::lowest();
        const auto hi = std::numeric_limits<long long>::max();
        std::vector<long long> a{lo};
        SegmentTreeBeats t(a);
        apply(t, a, 0, 1, hi, 1);
        checkRange(t, a, 0, 1);
        apply(t, a, 0, 1, lo, 0);
        checkRange(t, a, 0, 1);
        apply(t, a, 0, 1, hi, 2);
        checkRange(t, a, 0, 1);
        std::vector<long long> b{lo, 0};
        SegmentTreeBeats u(b);
        apply(u, b, 0, 2, -1, 1);
        checkRange(u, b, 0, 2);
        std::vector<long long> c{0, hi};
        SegmentTreeBeats v(c);
        apply(v, c, 0, 2, 1, 0);
        checkRange(v, c, 0, 2);
    });
    runCase("SegmentTreeBeats/wide-lazy-regression", [] {
        const auto high = std::numeric_limits<long long>::max();
        const auto low = std::numeric_limits<long long>::lowest();
        SegmentTreeBeats upper({high, 0});
        upper.chmin(0, 2, 10);
        upper.add(0, 2, 1);
        CHECK(upper.query(0, 1).sum == 11);
        CHECK(upper.query(1, 2).sum == 1);
        SegmentTreeBeats lower({low, 0});
        lower.chmax(0, 2, -10);
        lower.add(0, 2, -1);
        CHECK(lower.query(0, 1).sum == -11);
        CHECK(lower.query(1, 2).sum == -1);
        long long half = high / 2;
        for (long long sign : {-1LL, 1LL}) {
            SegmentTreeBeats tree({sign * (-2 * half), 0});
            tree.add(0, 2, sign * half);
            if (sign == 1) {
                tree.chmin(0, 2, 0);
            } else {
                tree.chmax(0, 2, 0);
            }
            tree.add(0, 2, sign * half);
            if (sign == 1) {
                tree.chmin(0, 2, 1);
            } else {
                tree.chmax(0, 2, -1);
            }
            tree.add(0, 2, sign * half);
            CHECK(tree.query(0, 1).sum == sign * half);
            CHECK(tree.query(1, 2).sum == sign * (half + 1));
            CHECK(tree.query(0, 2).sum == sign * high);
        }
    });
    runCase("SegmentTreeBeats/structured", [] {
        constexpr int n = 12001;
        std::vector<long long> a(n);
        for (int i = 0; i < n; ++i) {
            a[i] = i % 2 ? i : -i;
        }
        SegmentTreeBeats t(a);
        for (int step = 0; step < 40; ++step) {
            apply(t, a, 0, n, n - 100 * step, 0);
            apply(t, a, 0, n, -n + 100 * step, 1);
            apply(t, a, step, n - step, step - 20, 2);
            checkRange(t, a, step, n - step);
        }
        for (int i = 0; i < n; ++i) {
            checkRange(t, a, i, i + 1);
        }
    });
    return 0;
}
