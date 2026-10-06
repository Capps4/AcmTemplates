#include "../../../../src/Geometry/Geo3/code.hpp"
#include "../../../Support/BenchmarkSupport.hpp"
int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    const int n = input.n;
    std::mt19937_64 random(input.seed);
    std::vector<Point<double>> a;
    for (int i = 0; i < n; ++i)
        a.emplace_back(int(random() % 10000), int(random() % 10000),
                       input.shape ? 0 : int(random() % 10000));
    return measure(input, "3D line and plane distance; random / coplanar", [&]() -> std::uint64_t {
        Line<double> l{{0, 0, 0}, {1, 2, 3}};
        Plane<double> p{{0, 0, 0}, {1, 0, 0}, {0, 1, 0}};
        double sum = 0;
        for (auto x : a)
            sum += l.dist(x) + p.dist(x);
        return std::uint64_t(sum);
    });
}
