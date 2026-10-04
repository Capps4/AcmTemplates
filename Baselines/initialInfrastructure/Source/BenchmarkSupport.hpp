#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>

inline volatile std::uint64_t benchmarkSink = 0;
inline bool benchmarkReverse = false;
inline void benchmarkClobber() { asm volatile("" ::: "memory"); }
inline void benchmarkConsume(std::uint64_t x) { asm volatile("" : : "r"(x) : "memory"); }

template<class F>
double benchmarkBatch(F& run, int cnt, std::uint64_t expected) {
    std::uint64_t sum = 0;
    benchmarkClobber();
    auto start = std::chrono::steady_clock::now();
    benchmarkClobber();
    for (int i = 0; i < cnt; ++i) {
        auto res = std::uint64_t(run());
        benchmarkConsume(res);
        sum += res;
    }
    benchmarkClobber();
    auto stop = std::chrono::steady_clock::now();
    benchmarkSink ^= sum;
    if (sum != expected * std::uint64_t(cnt)) {
        std::cerr << "Benchmark invocation changed its checksum; reset mutable fixtures per call\n";
        std::exit(1);
    }
    return std::chrono::duration<double, std::milli>(stop - start).count();
}

inline void printSamples(const std::array<double, 7>& a, int cnt) {
    std::cerr << '[';
    for (int i = 0; i < cnt; ++i) {
        if (i) std::cerr << ',';
        std::cerr << a[i];
    }
    std::cerr << ']';
}

template<class Before, class After>
void compare(const char* name, Before&& before, After&& after, bool batch = true) {
    auto old = before(), res = after();
    if (old != res) {
        std::cerr << "Benchmark checksum mismatch: " << name << '\n';
        std::exit(1);
    }
    auto expected = std::uint64_t(res);
    const char* env = std::getenv("BENCH_MIN_MS");
    double minMs = env ? std::atof(env) : 10;
    const char* side = std::getenv("BENCH_SIDE");
    bool onlyFinal = side && side[0] == 'f';
    const char* rnd = std::getenv("BENCH_ROUNDS");
    int rounds = rnd ? std::atoi(rnd) : 7;
    if (rounds < 1 || rounds > 7) std::exit(1);
    int cnt = 1;
    // Same batch size for both versions. These calls also warm up the fixtures.
    while (true) {
        double a = onlyFinal ? minMs : benchmarkBatch(before, cnt, expected);
        double b = benchmarkBatch(after, cnt, expected);
        if (!batch || std::min(a, b) >= minMs || std::max(a, b) >= 100 || cnt == 1024) break;
        cnt *= 2;
    }
    std::array<double, 7> original{}, final{};
    for (int i = 0; i < rounds; ++i) {
        if (onlyFinal) {
            final[i] = benchmarkBatch(after, cnt, expected) / cnt;
            original[i] = final[i];
        } else if (benchmarkReverse != bool(i & 1)) {
            final[i] = benchmarkBatch(after, cnt, expected) / cnt;
            original[i] = benchmarkBatch(before, cnt, expected) / cnt;
        } else {
            original[i] = benchmarkBatch(before, cnt, expected) / cnt;
            final[i] = benchmarkBatch(after, cnt, expected) / cnt;
        }
    }
    auto a = original, b = final;
    for (int i = rounds; i < 7; ++i) {
        a[i] = a[rounds - 1];
        b[i] = b[rounds - 1];
    }
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    std::cout << std::setprecision(17) << name << ',' << a[3] << ',' << b[3] << ',';
    if (a[3] > 0 && b[3] > 0) std::cout << a[3] / b[3];
    else std::cout << "NA";
    std::cout << ',' << expected << '\n';
    std::cerr << std::setprecision(17) << "BENCHMARK_SAMPLES:{\"scenario\":" << std::quoted(name)
              << ",\"original\":";
    printSamples(original, rounds);
    std::cerr << ",\"final\":";
    printSamples(final, rounds);
    std::cerr << ",\"checksum\":" << expected << ",\"batch\":" << cnt
              << ",\"first\":\"" << (benchmarkReverse ? "final" : "original")
              << "\",\"method\":\"" << (onlyFinal ? "final-only batches" : "paired alternating batches") << "\"}\n";
    benchmarkReverse = !benchmarkReverse;
}
