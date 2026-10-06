#include "../../../../src/Geometry/Geo2/PointVec.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<Point<double>> a;
    for (int i = 0; i < n; ++i)
        a.emplace_back(input.shape ? i : int(random() % 100000),
                       input.shape ? i % 2 : int(random() % 100000));
    return measure(input, "geometry PointVec; random / narrow point cloud", [&]() -> std::uint64_t {
        double sum = 0;
        for (auto p : a)
            sum += p.toVec().dot(Vec<double>{3, 4}) + p.dist2(Point<double>{7, 9});
        return std::uint64_t(sum);
    });
}
