#pragma once
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

inline volatile std::uint64_t benchmarkSink = 0;
template<class F>
double measure(F&& run) {
    benchmarkSink ^= run();
    std::vector<double> samples;
    for (int trial = 0; trial < 7; ++trial) {
        auto start = std::chrono::steady_clock::now();
        auto result = run();
        auto stop = std::chrono::steady_clock::now();
        benchmarkSink ^= result;
        samples.push_back(std::chrono::duration<double, std::milli>(stop - start).count());
    }
    std::sort(samples.begin(), samples.end());
    return samples[samples.size() / 2];
}
template<class Before, class After>
void compare(const char* name, Before&& before, After&& after) {
    auto oldResult = before(), newResult = after();
    if (oldResult != newResult) {
        std::cerr << "Benchmark checksum mismatch: " << name << '\n';
        std::exit(1);
    }
    // Alternate timing order across scenarios to reduce systematic order bias.
    static bool reverse = false;
    double oldMs, newMs;
    if (reverse) { newMs = measure(after); oldMs = measure(before); }
    else { oldMs = measure(before); newMs = measure(after); }
    reverse = !reverse;
    std::cout << name << ',' << oldMs << ',' << newMs << ','
              << oldMs / newMs << ',' << newResult << '\n';
}
