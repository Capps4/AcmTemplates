#pragma once
#include "Include.hpp"

template <class T, class G = long double>
G closestPair(std::vector<Point<T>> ps) {
    std::sort(ps.begin(), ps.end());
    std::vector<Point<T>> buf(ps.size());
    G ans = std::numeric_limits<G>::max();
    auto sq = [](G x) {
        return x * x;
    };
    auto cmp = [](const auto& a, const auto& b) {
        return a.y < b.y;
    };
    auto work = [&](auto&& self, int l, int r) -> void {
        if (r - l <= 1) {
            return;
        }
        int mid = l + (r - l) / 2;
        T x = ps[mid].x;
        self(self, l, mid);
        self(self, mid, r);
        // Each child returns its points sorted by y; save the split x beforehand.
        std::merge(ps.begin() + l, ps.begin() + mid, ps.begin() + mid, ps.begin() + r,
                   buf.begin() + l, cmp);
        std::copy(buf.begin() + l, buf.begin() + r, ps.begin() + l);
        int cnt = 0;
        for (int i = l; i < r; ++i) {
            if (sq(G(ps[i].x) - G(x)) >= ans)
                continue;
            for (int j = cnt - 1; j >= 0 and sq(G(ps[i].y) - G(buf[j].y)) < ans; --j) {
                ans = std::min(ans, sq(G(ps[i].x) - G(buf[j].x)) + sq(G(ps[i].y) - G(buf[j].y)));
            }
            buf[cnt++] = ps[i];
        }
    };
    work(work, 0, int(ps.size()));
    return ans;
}
