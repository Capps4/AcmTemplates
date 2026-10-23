#include "../../../../../src/extra/DataStructures/MonotonyStack/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

int main(int argc, char** argv) {
    BenchmarkInput in(argc, argv);
std::vector<int> a(in.n);
    for (int i = 0; i < in.n; ++i) { a[i] = in.shape ? in.n-i : i; }
    return measure(in, "increasing / decreasing stack", [&]() -> std::uint64_t {
        MonotonyStack<int> st(-1,-1);
        std::uint64_t sum = 0;
        for (int i = 0; i < in.n; ++i) {
            int j = st.push(a[i],i);
            benchmarkCheck(j == (in.shape ? -1 : i-1), "nearest smaller");
            sum = benchmarkMix(sum, j + 1);
        }
        return sum;
    });
}
