#include "Workload.hpp"

using Matrix = std::vector<std::vector<Z>>;
Matrix mul(const Matrix &a, const Matrix &b) {
    int n = int(a.size());
    Matrix c(n, std::vector<Z>(n));
    for (int i = 0; i < n; ++i)
        for (int k = 0; k < n; ++k)
            for (int j = 0; j < n; ++j)
                c[i][j] += a[i][k] * b[k][j];
    return c;
}

int main(int argc, char **argv) {
    BenchmarkInput input(argc, argv);
    auto [a, c] = workload(input);
    Z expected = linearRecurrence(a, c, recurrenceIndex);
    return measure(input, "companion matrix baseline: same recurrence and index", [&] {
        int n = input.n;
        Matrix x(n, std::vector<Z>(n)), res(n, std::vector<Z>(n));
        for (int i = 0; i < n; ++i) {
            x[0][i] = c[i];
            res[i][i] = 1;
            if (i != 0)
                x[i][i - 1] = 1;
        }
        auto k = recurrenceIndex - std::uint64_t(n - 1);
        while (k != 0) {
            if (k & 1)
                res = mul(res, x);
            k >>= 1;
            if (k != 0)
                x = mul(x, x);
        }
        Z value{};
        for (int i = 0; i < n; ++i)
            value += res[0][i] * a[n - i - 1];
        benchmarkCheck(value == expected, "matrix and polynomial recurrence agree");
        return std::uint64_t(value.val());
    });
}
