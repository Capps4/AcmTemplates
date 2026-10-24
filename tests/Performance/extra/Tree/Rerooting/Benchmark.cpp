#include "../../../../../src/extra/Tree/Rerooting/code.hpp"
#include "Input.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    auto g = graph(in);
    using Info = std::pair<long long, long long>;
    return measure(in, "generic rerooting distance sums; chain / star", [&]() -> std::uint64_t {
        Rerooting a(g, Info{}, [](Info x, Info y) {
            return Info{x.first + y.first, x.second + y.second};
        }, [](Info x, int, int) {
            return Info{x.first, x.second + x.first};
        }, [](Info x, int) {
            return Info{x.first + 1, x.second};
        });
        std::uint64_t sum = 0;
        for (int x = 0; x < in.n; ++x) {
            benchmarkCheck(a.ans[x] == Info(in.n, expected(in, x)), "distance sum formula");
            sum = benchmarkMix(sum, a.ans[x].second);
        }
        return sum;
    });
}
