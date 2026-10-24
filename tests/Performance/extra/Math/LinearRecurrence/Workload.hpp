#pragma once
#include "../../../../../src/extra/Math/LinearRecurrence/code.hpp"
#include "../../../../../src/core/Math/Numeric/ModuloInteger/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

inline auto workload(const BenchmarkInput &input) {
    std::mt19937_64 rng(input.seed);
    std::vector<Z> a(input.n), c(input.n);
    for (int i = 0; i < input.n; ++i) {
        a[i] = rng();
        c[i] = input.shape and i != 0 and i != input.n - 1 ? 0 : rng();
    }
    return std::make_pair(a, c);
}

constexpr std::uint64_t recurrenceIndex = 1000000000000000000ULL;
