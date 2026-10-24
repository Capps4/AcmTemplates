#include "../../../../../src/extra/Geometry/MinimumEnclosingCircle/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    std::mt19937_64 rng(in.seed);
    std::vector<Point<double>> a{{-1, 0}, {1, 0}};
    if (in.n == 1) {
        a = {{0, 0}};
    } else if (in.shape == 0 and in.n >= 3) {
        auto y = std::sqrt(0.75);
        a = {{1, 0}, {-0.5, y}, {-0.5, -y}};
    }
    std::uniform_real_distribution<double> dis(-0.25, 0.25);
    for (int i = int(a.size()); i < in.n; ++i) {
        a.emplace_back(in.shape ? 2. * i / in.n - 1 : dis(rng), in.shape ? 0 : dis(rng));
    }
    return measure(in, "three-point support with interior cloud / collinear points", [&]() -> std::uint64_t {
        auto c = minimumEnclosingCircle(a, in.seed);
        double want = in.n == 1 ? 0 : 1;
        benchmarkCheck(c and std::abs(c->r - want) < 1e-10, "known radius");
        benchmarkCheck(c->o.dist2(Point<double>{}) < 1e-20, "known center");
        return std::uint64_t(std::llround(c->r * 1000000000));
    });
}
