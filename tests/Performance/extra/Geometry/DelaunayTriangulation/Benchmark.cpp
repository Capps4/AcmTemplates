#include "../../../../../src/extra/Geometry/DelaunayTriangulation/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<Point<long double>> ps;
    for (int i=1;i<=in.n;++i) { ps.emplace_back(i,1.L*i*i); }
    if (in.shape) { std::mt19937_64 rng(in.seed); std::shuffle(ps.begin(),ps.end(),rng); }
    return measure(in, "ordered / shuffled general-position parabola", [&]() -> std::uint64_t {
        Delaunay d(ps);
        std::uint64_t sum=0;
        int cnt=0;
        for (auto e:d.face) {
            int a=e->from,b=e->r->from,c=e->r->r->from;
            if (a<in.n and b<in.n and c<in.n) { ++cnt; sum=benchmarkMix(sum,a+b+c); }
        }
        benchmarkCheck(cnt==in.n-2,"convex point triangulation");
        return sum;
    });
}
