#include "../../../../../src/extra/Geometry/Geo3/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<Point<double>> a;
    for (int i = 0; i < n; ++i)
        a.emplace_back(int(random() % 10000), int(random() % 10000),
                       input.shape ? 0 : int(random() % 10000));
    long double want = 0;
    for (auto p : a) {
        long double x = p.x, y = p.y, z = p.z;
        auto dot = x + 2 * y + 3 * z;
        auto d = x * x + y * y + z * z - dot * dot / 14;
        want += std::sqrt(std::max(0.L, d)) + std::abs(z);
    }
    return measure(input, "3D line and plane distance; random / coplanar", [&]() -> std::uint64_t {
        Line<double> l{{0, 0, 0}, {1, 2, 3}};
        Plane<double> p{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
        double sum = 0;
        for (auto x : a)
            sum += l.dist(x) + p.dist(x);
        benchmarkCheck(std::isfinite(sum) and std::abs(sum - want) <= 1e-10L * std::max(1.L, want),
                       "independent line and plane distance");
        return std::uint64_t(std::llround(sum * 1000));
    });
}
