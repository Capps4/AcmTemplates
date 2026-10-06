#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>
#include <vector>

inline volatile std::uint64_t benchmarkSink = 0;
#ifndef BENCHMARK_REPEATS
#define BENCHMARK_REPEATS 3
#endif
#ifndef BENCHMARK_ITERATIONS
#define BENCHMARK_ITERATIONS 1
#endif

inline std::uint64_t benchmarkMix(std::uint64_t state, std::uint64_t value) {
    return (state ^ (value + 0x9e3779b97f4a7c15ULL)) * 0x100000001b3ULL;
}

struct BenchmarkInput {
    int n = 10000;
    int shape = 0;
    std::uint64_t seed = 20261005;
    explicit BenchmarkInput(int argc, char **argv) {
        if (argc > 1)
            n = std::stoi(argv[1]);
        if (argc > 2)
            shape = std::stoi(argv[2]);
        if (argc > 3)
            seed = std::stoull(argv[3]);
        if (n < 1 or shape < 0 or shape > 1)
            std::exit(2);
    }
};

inline void benchmarkCheck(bool passed, const char *invariant) {
    if (not passed) {
        std::cerr << "BENCHMARK_FAIL " << invariant << std::endl;
        std::exit(1);
    }
}

template <class F>
int measure(const BenchmarkInput &input, const char *workload, F work) {
    constexpr int repeats = BENCHMARK_REPEATS;
    static_assert(repeats >= 3 and repeats % 2 == 1);
    std::vector<double> elapsed;
    constexpr int iterations = BENCHMARK_ITERATIONS;
    static_assert(iterations >= 1);
    auto batch = [&]() {
        std::uint64_t checksum = 0;
        for (int i = 0; i < iterations; ++i) {
            auto value = work();
            benchmarkSink = value;
            checksum = benchmarkMix(checksum, value);
        }
        return checksum;
    };
    auto warmBegin = std::chrono::steady_clock::now();
    std::uint64_t expected = batch();
    auto warmEnd = std::chrono::steady_clock::now();
    double warmup = std::chrono::duration<double, std::milli>(warmEnd - warmBegin).count();
    benchmarkSink = expected;
    for (int i = 0; i < repeats; ++i) {
        auto begin = std::chrono::steady_clock::now();
        auto checksum = batch();
        auto end = std::chrono::steady_clock::now();
        benchmarkSink = checksum;
        if (checksum != expected) {
            std::cerr << "Unstable benchmark checksum" << std::endl;
            return 1;
        }
        elapsed.push_back(std::chrono::duration<double, std::milli>(end - begin).count());
    }
    auto sorted = elapsed;
    std::sort(sorted.begin(), sorted.end());
    std::cout.precision(17);
    std::cout << "{\"workload\":\"" << workload << "\",\"n\":" << input.n
              << ",\"shape\":" << input.shape << ",\"seed\":" << input.seed
              << ",\"iterations\":" << iterations
              << ",\"checksum\":" << expected << ",\"medianMs\":" << sorted[repeats / 2]
              << ",\"warmupMs\":" << warmup
              << ",\"minMs\":" << sorted.front() << ",\"maxMs\":" << sorted.back()
              << ",\"samplesMs\":[";
    for (int i = 0; i < repeats; ++i) {
        if (i)
            std::cout << ',';
        std::cout << elapsed[i];
    }
    std::cout << "]}" << std::endl;
    return 0;
}
