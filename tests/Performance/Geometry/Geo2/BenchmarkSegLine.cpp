#include "../../../../src/Geometry/Geo2/SegLine.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<Point<double>> a;
    for (int i = 0; i < n; ++i)
        a.emplace_back(input.shape ? i : int(random() % 100000),
                       input.shape ? i % 2 : int(random() % 100000));
    return measure(input, "geometry SegLine; random / narrow point cloud", [&]() -> std::uint64_t {
        Line<double> l{{0, 0}, {3, 4}};
        double sum = 0;
        for (auto p : a) {
            auto q = near(p, l);
            sum += q.first.dist2(q.second);
        }
        return std::uint64_t(sum);
    });
}
