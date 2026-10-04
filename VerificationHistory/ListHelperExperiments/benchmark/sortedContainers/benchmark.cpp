#include "Before.hpp"
#include "Generic.hpp"
#include "Unified.hpp"
#include "../../ListHelperFinal.cpp"
#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <random>
#include <stdexcept>

using Clock = std::chrono::steady_clock;

template<int Method, class List, class Compare>
__attribute__((noinline, aligned(64)))
void run(List& a, Compare compare) {
    using T = typename List::value_type;
    if constexpr (Method == 0)
        a = std::move(a) | beforeSeq::sorted(compare);
    if constexpr (Method == 1)
        a = std::move(a) | genericSeq::sorted(compare);
    if constexpr (Method == 2)
        a = std::move(a) | unifiedSeq::sorted(compare);
    if constexpr (Method == 6)
        a = std::move(a) | seq::sorted(compare);
    if constexpr (Method == 5)
        std::sort(a.begin(), a.end(), compare);
    if constexpr (Method == 3) {
        constexpr bool ascending = std::is_same_v<Compare, std::less<T>> ||
                                   std::is_same_v<Compare, std::less<>>;
        if constexpr (ascending)
            std::sort(a.begin(), a.end());
        else
            std::sort(a.begin(), a.end(), std::greater<>{});
    }
    if constexpr (Method == 4)
        std::sort(a.begin(), a.end(), std::ref(compare));
}

template<class List, class Compare>
void measure(const List& input, Compare compare, const char* container,
             const char* type, const char* distribution, const char* comparator,
             bool fallback, std::mt19937_64& random) {
    auto expected = std::make_unique<List>(input);
    std::sort(expected->begin(), expected->end(), compare);
    using Fn = void (*)(List&, Compare);
    std::vector<std::pair<int, Fn>> methods;
    if (fallback)
        methods = {{3, run<3, List, Compare>}, {4, run<4, List, Compare>},
                   {5, run<5, List, Compare>}};
    else
        methods = {{0, run<0, List, Compare>}, {1, run<1, List, Compare>},
                   {2, run<2, List, Compare>}, {6, run<6, List, Compare>}};
    for (int round = -1; round < 7; ++round) {
        std::shuffle(methods.begin(), methods.end(), random);
        for (auto [method, function] : methods) {
            double total = 0;
            int repeats = 0;
            do {
                auto a = std::make_unique<List>(input);
                std::atomic_signal_fence(std::memory_order_seq_cst);
                auto start = Clock::now();
                function(*a, compare);
                auto stop = Clock::now();
                std::atomic_signal_fence(std::memory_order_seq_cst);
                if (*a != *expected)
                    throw std::runtime_error("output mismatch");
                total += std::chrono::duration<double, std::nano>(stop - start).count();
                ++repeats;
            } while (total < 1000000 && repeats < 512);
            if (round >= 0)
                std::cout << (fallback ? "fallback" : "pipeline") << ',' << container << ','
                          << type << ',' << input.size() << ',' << distribution << ','
                          << comparator << ',' << method << ',' << round << ','
                          << repeats << ',' << total / repeats << '\n';
        }
    }
}

template<class T, class List>
void fill(List& a, int kind, std::mt19937_64& random) {
    using U = std::make_unsigned_t<T>;
    for (auto& x : a) {
        U bits = U(random());
        if (kind == 1)
            bits = U(random() % (1 << 22));
        if (kind == 2)
            bits = U(random() % 256) << 16;
        std::memcpy(&x, &bits, sizeof(T));
    }
    if (kind >= 3) {
        std::sort(a.begin(), a.end());
        if (kind == 4)
            std::reverse(a.begin(), a.end());
        if (kind == 5)
            for (std::size_t j = 0; j < a.size() / 64 + 1; ++j)
                std::swap(a[random() % a.size()], a[random() % a.size()]);
    }
}

template<class T>
void sweep(const char* type, std::mt19937_64& random) {
    const char* names[] = {"full", "bits22", "gaps", "ordered", "reverse", "nearly"};
    for (int n : {128, 512, 4096, 1000000}) {
        for (int kind : {0, 1, 2, 3, 4, 5}) {
            std::vector<T> a(n);
            fill<T>(a, kind, random);
            measure(a, std::less<>{}, "vector", type, names[kind], "lessVoid", true, random);
            measure(a, std::less<T>{}, "vector", type, names[kind], "lessTyped", true, random);
            measure(a, std::greater<T>{}, "vector", type, names[kind], "greaterTyped", true, random);
            if (kind <= 2 || (n == 1000000 && kind >= 3)) {
                measure(a, std::less<>{}, "vector", type, names[kind], "lessVoid", false, random);
                measure(a, std::greater<T>{}, "vector", type, names[kind], "greaterTyped", false, random);
            }
        }
    }
    for (int kind : {0, 1, 2}) {
        auto a = std::make_unique<std::array<T, 65536>>();
        fill<T>(*a, kind, random);
        measure(*a, std::less<>{}, "array", type, names[kind], "lessVoid", false, random);
        measure(*a, std::greater<T>{}, "array", type, names[kind], "greaterTyped", false, random);
    }
}

int main() {
    std::mt19937_64 random(177613);
    std::cout << "kind,container,type,n,distribution,comparator,method,round,repeats,ns\n"
              << std::fixed << std::setprecision(3);
    sweep<std::int32_t>("i32", random);
    sweep<std::int64_t>("i64", random);
}
