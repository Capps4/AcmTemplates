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

template <class F>
int measure(const BenchmarkInput &input, const char *workload, F work) {
    constexpr int repeats = 7;
    std::vector<double> elapsed;
    std::uint64_t expected = work(); // Warmup, outside measured samples.
    benchmarkSink = expected;
    for (int i = 0; i < repeats; ++i) {
        auto begin = std::chrono::steady_clock::now();
        auto checksum = work();
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
              << ",\"checksum\":" << expected << ",\"medianMs\":" << sorted[repeats / 2]
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
