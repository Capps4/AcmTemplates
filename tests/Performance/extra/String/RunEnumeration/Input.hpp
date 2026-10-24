#pragma once
#include "../../../../../src/extra/String/RunEnumeration/code.hpp"
#include "../../../../Support/BenchmarkSupport.hpp"

inline std::string workload(const BenchmarkInput &input) {
    std::string s;
    if (input.shape) {
        std::string a = "a", b = "ab";
        while (int(b.size()) < input.n) {
            std::string c = b + a;
            a = std::move(b);
            b = std::move(c);
        }
        s = b.substr(0, input.n);
    } else {
        std::mt19937_64 rng(input.seed);
        s.resize(input.n);
        for (auto &c : s)
            c = char(rng() % 4);
    }
    return s;
}

inline std::uint64_t checksum(const std::vector<std::tuple<int, int, int>> &runs,
                              std::string_view s) {
    benchmarkCheck(runs.size() < s.size() or s.empty(), "linear run-count bound");
    std::uint64_t sum = 0;
    for (auto [p, l, r] : runs) {
        benchmarkCheck(r - l >= 2 * p, "at least two periods");
        benchmarkCheck(l == 0 or s[l - 1] != s[l - 1 + p], "left maximality");
        benchmarkCheck(r == int(s.size()) or s[r] != s[r - p], "right maximality");
        sum = benchmarkMix(sum, p);
        sum = benchmarkMix(sum, l);
        sum = benchmarkMix(sum, r);
    }
    return sum;
}
