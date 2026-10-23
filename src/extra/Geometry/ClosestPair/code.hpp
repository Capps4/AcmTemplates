#pragma once
#include "Include.hpp"

template <class T, class G = long double>
G closestPair(std::vector<Point<T>> ps) {
    std::sort(ps.begin(), ps.end());
    G ans = std::numeric_limits<G>::max();
    auto sq = [](G x) { return x * x; };
    auto work = [&](auto&& self, int l, int r) -> void {
        if (r - l <= 1) {
            return;
        }
        int mid = l + (r - l) / 2;
        self(self, l, mid);
        self(self, mid, r);
        std::vector<Point<T>> b{};
        for (int i = l; i < r; ++i) {
            if (sq(G(ps[i].x) - G(ps[mid].x)) < ans) {
                b.push_back(ps[i]);
            }
        }
        std::sort(b.begin(), b.end(), [](const auto& x, const auto& y) { return x.y < y.y; });
        for (int i = 0; i < int(b.size()); ++i) {
            for (int j = i + 1; j < int(b.size()) and sq(G(b[j].y) - G(b[i].y)) < ans; ++j) {
                ans = std::min(ans, sq(G(b[j].x) - G(b[i].x)) + sq(G(b[j].y) - G(b[i].y)));
            }
        }
    };
    work(work, 0, int(ps.size()));
    return ans;
}
