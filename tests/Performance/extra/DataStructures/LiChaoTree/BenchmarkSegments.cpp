#include "../../../../../src/extra/DataStructures/LiChaoTree/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    struct Line {
        using Key = long long;
        using Value = long long;
        long long k, b;
        long long operator[](long long x) const { return k * x + b; }
    };
    std::vector<long long> xs(in.n);
    std::iota(xs.begin(), xs.end(), 0LL);
    std::mt19937_64 rng(in.seed);
    struct Segment {
        int l, r;
        Line line;
    };
    std::vector<Segment> segments;
    int width = in.shape ? std::max(1, in.n / 2) : 8;
    long long boost = 4LL * in.n * in.n;
    for (int i = 0; i < in.n; ++i) {
        int block = i / width, lo = block * width, hi = std::min(in.n, lo + width);
        int l = lo + int(rng() % (i - lo + 1));
        int r = i + 1 + int(rng() % (hi - i));
        segments.push_back({l, r, {2LL * i, -1LL * i * i + (block % 2) * boost}});
    }
    std::shuffle(segments.begin(), segments.end(), rng);
    std::uint64_t expected = 0;
    for (int x = 0; x < in.n; ++x) {
        expected = benchmarkMix(expected, 1LL * x * x + (x / width % 2) * boost);
    }
    return measure(in, "short / wide Li Chao segments", [&]() -> std::uint64_t {
        LiChaoTree<Line> t(xs);
        // Each tangent covers its own key, so each block has envelope x*x + its boost.
        // Extending boosted lines outside their block would corrupt the neighboring block.
        for (const auto& segment : segments) {
            t.insert(segment.l, segment.r, segment.line);
        }
        std::uint64_t sum = 0;
        for (int x = 0; x < in.n; ++x) {
            sum = benchmarkMix(sum, t.query(x));
        }
        benchmarkCheck(sum == expected, "segment envelope checksum");
        return sum;
    });
}
