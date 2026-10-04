#include "Candidates.hpp"
#include "../../Sorted.hpp"
#include <atomic>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <numeric>
#include <random>

using Clock = std::chrono::steady_clock;

template<int Method, class T, class Compare>
__attribute__((noinline, aligned(64)))
void run(std::vector<T>& a, Compare compare) {
    if constexpr (Method == 0)
        a = std::move(a) | baselineSeq::sorted(compare);
    else if constexpr (Method == 1)
        a = std::move(a) | candidates::sorted<8, false>(compare);
    else if constexpr (Method == 2)
        a = std::move(a) | candidates::sorted<8, true>(compare);
    else if constexpr (Method == 3)
        a = std::move(a) | candidates::sorted<11, true>(compare);
    else if constexpr (Method == 4)
        a = std::move(a) | candidates::sorted<15, true>(compare);
    else if constexpr (Method == 6)
        a = std::move(a) | seq::sorted(compare);
    else
        std::sort(a.begin(), a.end(), compare);
}

template<class T, class Compare>
void measure(const char* type, int n, int bits, int gaps, int pattern,
             Compare compare, const char* comparator, int stage, std::uint64_t seed) {
    using U = std::make_unsigned_t<T>;
    std::mt19937_64 random(seed + n + bits + gaps * 41 + pattern * 71);
    std::vector<T> input(n);
    const U mask = bits >= int(sizeof(T) * 8) ? std::numeric_limits<U>::max() : U((U(1) << bits) - 1);
    for (auto& x : input) {
        U value = (U(random()) & mask) << gaps;
        if (bits + gaps < int(sizeof(T) * 8))
            value += U(std::numeric_limits<T>::min());
        std::memcpy(&x, &value, sizeof(T));
    }
    auto expected = input;
    std::sort(expected.begin(), expected.end(), compare);
    if (pattern) {
        input = expected;
        if (pattern == 2)
            std::reverse(input.begin(), input.end());
        if (pattern == 3)
            for (int i = 0; i < n / 64 + 1; ++i)
                std::swap(input[random() % n], input[random() % n]);
    }
    using Fn = void (*)(std::vector<T>&, Compare);
    std::vector<std::pair<int, Fn>> methods;
    if (stage == 1)
        methods = {{0, run<0, T, Compare>}, {1, run<1, T, Compare>}, {5, run<5, T, Compare>}};
    if (stage == 2)
        methods = {{1, run<1, T, Compare>}, {2, run<2, T, Compare>}};
    if (stage == 3)
        methods = {{2, run<2, T, Compare>}, {3, run<3, T, Compare>}, {4, run<4, T, Compare>}};
    if (stage == 4)
        methods = {{0, run<0, T, Compare>}, {2, run<2, T, Compare>},
                   {6, run<6, T, Compare>}, {5, run<5, T, Compare>}};
    for (int round = -1; round < 5; ++round) {
        std::shuffle(methods.begin(), methods.end(), random);
        for (auto [method, function] : methods) {
            double total = 0;
            int repeats = 0;
            do {
                auto a = input;
                std::atomic_signal_fence(std::memory_order_seq_cst);
                auto start = Clock::now();
                function(a, compare);
                auto stop = Clock::now();
                std::atomic_signal_fence(std::memory_order_seq_cst);
                if (a != expected)
                    throw std::runtime_error("sort result mismatch");
                total += std::chrono::duration<double, std::nano>(stop - start).count();
                ++repeats;
            } while (total < 1000000 && repeats < 256);
            if (round >= 0)
                std::cout << type << ',' << n << ',' << bits << ',' << gaps << ',' << pattern << ','
                          << comparator << ',' << method << ',' << round + 1 << ',' << repeats << ','
                          << total / repeats << '\n';
        }
    }
}

template<class T>
void sweep(const char* type, int stage) {
    if (stage == 1) {
        for (int n : {128, 1024, 1000000})
            for (int bits : {4, 22, int(sizeof(T) * 8)}) {
                measure<T>(type, n, bits, 0, 0, std::less<>{}, "lessVoid", stage, 4213);
                measure<T>(type, n, bits, 0, 0, std::less<T>{}, "lessTyped", stage, 4213);
                measure<T>(type, n, bits, 0, 0, std::greater<>{}, "greaterVoid", stage, 4213);
                measure<T>(type, n, bits, 0, 0, std::greater<T>{}, "greaterTyped", stage, 4213);
            }
    } else if (stage == 2) {
        for (int n : {1024, 65536, 1000000})
            for (int gaps : {0, 8, 16}) {
                measure<T>(type, n, 8, gaps, 0, std::less<>{}, "lessVoid", stage, 9913);
                measure<T>(type, n, 8, gaps, 0, std::greater<>{}, "greaterVoid", stage, 9913);
            }
    } else if (stage == 3) {
        for (int n : {256, 1024, 4096, 16384, 65536, 262144, 1000000, 2000000}) {
            for (int bits : {4, 16, 22, int(sizeof(T) * 8)})
                measure<T>(type, n, bits, 0, 0, std::less<>{}, "lessVoid", stage, 83017);
            measure<T>(type, n, 8, 16, 0, std::less<>{}, "lessVoid", stage, 83017);
        }
    } else {
        for (int n : {384, 1536, 4095, 4096, 4097, 32768, 131072, 750000}) {
            for (int bits : {12, 20, 24, int(sizeof(T) * 8)})
                measure<T>(type, n, bits, 0, 0, std::less<>{}, "lessVoid", stage, 671923);
            measure<T>(type, n, 8, 16, 0, std::less<>{}, "lessVoid", stage, 671923);
        }
        for (int pattern : {1, 2, 3})
            measure<T>(type, 65536, 20, 0, pattern, std::less<>{}, "lessVoid", stage, 671923);
        for (int pattern : {0, 1, 2, 3}) {
            measure<T>(type, 1000000, 22, 0, pattern, std::less<>{}, "lessVoid", stage, 671923);
            measure<T>(type, 1000000, 22, 0, pattern, std::less<T>{}, "lessTyped", stage, 671923);
            measure<T>(type, 1000000, 22, 0, pattern, std::greater<T>{}, "greaterTyped", stage, 671923);
        }
        measure<T>(type, 1000000, 8, 16, 0, std::less<>{}, "lessVoid", stage, 671923);
        measure<T>(type, 1000000, int(sizeof(T) * 8), 0, 0, std::less<>{}, "lessVoid", stage, 671923);
    }
}

int main(int argc, char** argv) {
    if (argc != 2)
        return 2;
    int stage = std::stoi(argv[1]);
    std::cout << "type,n,bits,gaps,pattern,comparator,method,round,repeats,ns\n"
              << std::fixed << std::setprecision(3);
    sweep<std::int32_t>("i32", stage);
    sweep<std::int64_t>("i64", stage);
}
