#include "../../../../../src/extra/DataStructures/SlidingWindowAggregation/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
    int width = std::max(1, in.n / 8);
    return measure(in, "rolling / batch drain window aggregation", [&]() -> std::uint64_t {
        SWAG<long long> q;
        std::uint64_t sum = 0;
        int first = 0;
        for (int x = 0; x < in.n; ++x) {
            q.push(x);
            if (in.shape == 0 and q.size() > std::size_t(width)) {
                q.pop();
                ++first;
            }
            long long expected = 1LL * (first + x) * (x - first + 1) / 2;
            benchmarkCheck(q.query() == expected, "arithmetic window sum");
            sum = benchmarkMix(sum, q.query());
            if (in.shape and q.size() == std::size_t(width)) {
                while (not q.empty()) {
                    benchmarkCheck(q.query() == expected, "draining window sum");
                    sum = benchmarkMix(sum, q.query());
                    q.pop();
                    expected -= first;
                    ++first;
                }
            }
        }
        return sum;
    });
}
