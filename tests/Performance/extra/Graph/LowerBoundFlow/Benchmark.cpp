#include "../../../../../src/extra/Graph/LowerBoundFlow/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char **argv) {
    BenchmarkInput in(argc, argv);
    return measure(in, "bounded parallel paths / reverse edges", [&]() -> std::uint64_t {
        LowerBoundFlow<long long> a(in.n + 2);
        int s = in.n, t = s + 1;
        for (int x = 0; x < in.n; ++x) {
            a.add(s, x, 1, 5);
            a.add(x, t, 1, 3);
            if (in.shape)
                a.add(x, s, 1, 2);
        }
        auto val = a.work(s, t);
        benchmarkCheck(val == 3LL * in.n, "bounded maximum flow");
        std::uint64_t sum = 0;
        int stride = in.shape ? 3 : 2;
        for (int x = 0; x < in.n; ++x) {
            auto first = a.get(stride * x), second = a.get(stride * x + 1);
            auto back = in.shape ? a.get(stride * x + 2) : 0;
            benchmarkCheck(second == 3 and first - back == 3, "recovered conservation");
            sum = benchmarkMix(sum, second);
        }
        return sum;
    });
}
