#include "Workload.hpp"

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    auto [a, c] = workload(input);
    int n = input.n;
    for (int i = n; i < 2 * n; ++i) {
        Z value{};
        for (int j = 0; j < n; ++j)
            value += c[j] * a[i - j - 1];
        a.push_back(value);
    }
    Z expected = linearRecurrence(a, c, recurrenceIndex);
    return measure(input, "Berlekamp-Massey recovery and extrapolation from 2d terms", [&] {
        auto inferred = berlekampMassey(a);
        Z value = linearRecurrence(a, inferred, recurrenceIndex);
        benchmarkCheck(value == expected, "recovered recurrence extrapolates");
        return std::uint64_t(value.val());
    });
}
