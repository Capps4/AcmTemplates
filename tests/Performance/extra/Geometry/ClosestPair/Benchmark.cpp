#include "../../../../../src/extra/Geometry/ClosestPair/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

#ifdef CLOSEST_PAIR_SORT_BASELINE
// The previous O(n log^2 n) merge step, retained only as a measured reference.
template <class T, class G>
G solve(std::vector<Point<T>> ps) {
    std::sort(ps.begin(), ps.end());
    G ans = std::numeric_limits<G>::max();
    auto sq = [](G x) { return x * x; };
    auto work = [&](auto&& self, int l, int r) -> void {
        if (r - l <= 1)
            return;
        int mid = l + (r - l) / 2;
        self(self, l, mid);
        self(self, mid, r);
        std::vector<Point<T>> b;
        for (int i = l; i < r; ++i)
            if (sq(G(ps[i].x) - G(ps[mid].x)) < ans)
                b.push_back(ps[i]);
        std::sort(b.begin(), b.end(), [](auto x, auto y) { return x.y < y.y; });
        for (int i = 0; i < int(b.size()); ++i)
            for (int j = i + 1; j < int(b.size()) and sq(G(b[j].y) - G(b[i].y)) < ans; ++j)
                ans = std::min(ans, sq(G(b[j].x) - G(b[i].x)) + sq(G(b[j].y) - G(b[i].y)));
    };
    work(work, 0, int(ps.size()));
    return ans;
}
#endif

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<Point<long long>> a;
    int w = int(std::sqrt(in.n)) + 1;
    for (int i = 0; i < in.n; ++i)
        a.emplace_back(in.shape ? 0 : 10LL * (i % w), in.shape ? i : 10LL * (i / w));
    std::shuffle(a.begin(), a.end(), rng);
    long long expected = in.n < 2 ? std::numeric_limits<long long>::max() : in.shape ? 1 : 100;
    return measure(in, "shuffled grid / vertical strip closest points", [&]() -> std::uint64_t {
#ifdef CLOSEST_PAIR_SORT_BASELINE
        auto ans = solve<long long, long long>(a);
#else
        auto ans = closestPair<long long, long long>(a);
#endif
        benchmarkCheck(ans == expected, "known lattice spacing");
        return std::uint64_t(ans);
    });
}
